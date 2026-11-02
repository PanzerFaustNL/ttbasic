/* TT BASIC 0.4 - ANSI C interpreter for Pure C / Atari TT. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "BASIC.H"
#define LIMIT 1000000L
#define PROGRAM_LINES 2048
#define NAME_LEN 24
#define CLASSES 16
#define FIELDS 16
#define METHODS 64
#define PARAMS 8
#define OBJECTS 64
#define SYMBOLS 64
#define CALL_DEPTH 8
#define CONTAINERS 32
#define ITEMS 128
#define STRING_LEN 128
#define BLOCKS 32
#define NUMBER 0
#define REFERENCE 1
#define STRING 2
#define ARRAY 3
#define COLLECTION 4
#define T_NUMBER -1
#define T_STRING -2
#define T_ANY -3
#define T_BOOLEAN -4
#define T_COLLECTION -5
#define T_VOID -6
#define T_OBJECT -7
#define ARRAY_TYPE 110
#define B_IF 1
#define B_WHILE 2
#define B_FOR 3
#define B_DO 4
#define B_SELECT 5

typedef struct { char text[LINE_LEN]; int file, row; } Code;
typedef struct { char path[BASIC_PATH]; int state, origin; } Module;
typedef struct { int kind, object, view; long number; char string[STRING_LEN]; } Value;
typedef struct { char name[NAME_LEN]; int type, declaration; Value value; } Symbol;
typedef struct { char name[NAME_LEN]; int visible, owner, type; Value initial; } Field;
typedef struct {
    char name[NAME_LEN], base_name[NAME_LEN], contracts[4][NAME_LEN];
    int base, is_interface, contract_count, fields, declaration, resolved;
    unsigned long interfaces;
    Field field[FIELDS];
} Class;
typedef struct {
    char name[NAME_LEN], params[PARAMS][NAME_LEN];
    char contract[NAME_LEN], contract_member[NAME_LEN];
    int owner, visible, function, argc, start, end, declaration;
    int parameter_type[PARAMS], result_type, virtual_method, overrides;
} Method;
typedef struct { int type; Value field[FIELDS]; } Instance;
typedef struct { int self, method, count; Symbol vars[SYMBOLS]; } Frame;
typedef struct { int kind, element_type, count; long lower, revision; Value item[ITEMS]; } Container;
static Code code[PROGRAM_LINES];
static Module modules[BASIC_FILES];
static Class classes[CLASSES];
static Method methods[METHODS];
static Instance objects[OBJECTS];
static Container containers[CONTAINERS];
static Frame frames[CALL_DEPTH + 1];
static int skip_to[PROGRAM_LINES], block_end[PROGRAM_LINES], branch_next[PROGRAM_LINES];
static int ncode, nfiles, nclasses, nmethods, nobjects, ncontainers, level;
static int pc, failure_file, failure_row, expr_depth, returned, stopped;
static int signal_kind, signal_continue;
static long steps;
static Value return_value;
static char *scan, *problem;
static char (*sink)[OUT_LEN];
static int *sink_used;
static BasicReader file_reader;
static BasicInput input_reader;
static BasicPoll poll_reader;
static Value expression(void);
static Value factor(void);
static Value invoke(int method, int self, Value *args, int argc, int need_value);
static void run_range(int start, int end);
static void simple(char *text);
static void initialize(int type, int object, Value *args, int argc, int caller);
static int check_type(int type, Value value);
static int method_index(int owner, char *name);
static int class_index(char *name);
void basic_set_reader(BasicReader reader) { file_reader = reader; }
void basic_set_input(BasicInput reader) { input_reader = reader; }
void basic_set_poll(BasicPoll reader) { poll_reader = reader; }
static Value number(long n)
{
    Value v; memset(&v, 0, sizeof(v)); v.kind = NUMBER; v.number = n;
    v.object = v.view = -1; return v;
}
static void fail(char *message)
{
    if (problem) return;
    problem = message;
    if (pc >= 0 && pc < ncode) { failure_file = code[pc].file; failure_row = code[pc].row; }
}
static int tick(void)
{
    if (++steps > 1000000L) fail("Execution limit reached (1000000 steps)");
    if (!problem && !(steps % 64) && poll_reader && !poll_reader()) fail("Execution cancelled");
    return !problem;
}
static void space(void) { while (*scan == ' ' || *scan == '\t') ++scan; }
static int equal(char *a, char *b)
{
    while (*a && *b) if (toupper((unsigned char)*a++) != toupper((unsigned char)*b++)) return 0;
    return *a == *b;
}
static int word(char *s)
{
    int i;
    space();
    for (i = 0; s[i]; ++i) if (toupper((unsigned char)scan[i]) != s[i]) return 0;
    if (isalnum((unsigned char)scan[i]) || scan[i] == '_') return 0;
    scan += i; return 1;
}
int basic_is_reserved(char *name)
{
    static char words[] = "IF THEN ELSE ELSEIF END WHILE WEND FOR EACH IN TO STEP NEXT SELECT CASE IS ISNOT DO LOOP UNTIL EXIT CONTINUE DIM AS NEW CLASS INTERFACE INHERITS IMPLEMENTS PUBLIC PRIVATE PROTECTED OVERRIDABLE OVERRIDES SUB FUNCTION RETURN CALL LET PRINT INPUT REM INTEGER LONG BOOLEAN STRING VARIANT OBJECT COLLECTION TRUE FALSE NOTHING ME MYBASE AND OR NOT XOR MOD REDIM PRESERVE TYPEOF ARRAY";
    char *p; int i;
    for (p = words; *p; ) {
        for (i = 0; p[i] && p[i] != ' '; ++i) { }
        if ((int)strlen(name) == i) {
            int j;
            for (j = 0; j < i; ++j) if (toupper((unsigned char)name[j]) != p[j]) break;
            if (j == i) return 1;
        }
        p += i; if (*p) ++p;
    }
    return 0;
}
static int ident(char *name)
{
    int n; space(); n = 0;
    if (!isalpha((unsigned char)*scan) && *scan != '_') { fail("Expected identifier"); return 0; }
    while (isalnum((unsigned char)*scan) || *scan == '_') {
        if (n == NAME_LEN - 1) { fail("Identifier too long (max 23)"); return 0; }
        name[n++] = (char)toupper((unsigned char)*scan++);
    }
    name[n] = 0; return 1;
}
static int declaration_name(char *name, int constructor)
{
    if (!ident(name)) return 0;
    if (basic_is_reserved(name) && !(constructor && !strcmp(name, "NEW"))) {
        fail("Reserved word cannot be declared as a name"); return 0;
    }
    return 1;
}
static int endline(void)
{
    space(); if (!*scan || *scan == '\'') return 1;
    fail("Unexpected text at end of statement"); return 0;
}
static void emit(char *text)
{
    if (*sink_used == OUT_LINES) { memmove(sink[0], sink[1], (OUT_LINES - 1) * OUT_LEN); --*sink_used; }
    strncpy(sink[*sink_used], text, OUT_LEN - 1); sink[*sink_used][OUT_LEN - 1] = 0; ++*sink_used;
}
static long numeric(Value value)
{
    if (value.kind != NUMBER) { fail("Expected numeric expression"); return 0; }
    return value.number;
}
static Value string_value(char *text)
{
    Value v; v = number(0); v.kind = STRING;
    if (strlen(text) >= STRING_LEN) fail("String exceeds 127 characters");
    else strcpy(v.string, text);
    return v;
}
static Value string_literal(void)
{
    char text[STRING_LEN]; int n;
    n = 0; ++scan;
    while (*scan) {
        if (*scan == '"') {
            ++scan;
            if (*scan != '"') { text[n] = 0; return string_value(text); }
        }
        if (n == STRING_LEN - 1) { fail("String exceeds 127 characters"); return number(0); }
        text[n++] = *scan++;
    }
    fail("Missing closing quote"); return number(0);
}
static Value default_value(int type)
{
    Value v; v = number(0);
    if (type == T_STRING) v.kind = STRING;
    else if (type == T_COLLECTION) v.kind = COLLECTION;
    else if (type >= ARRAY_TYPE - 10) v.kind = ARRAY;
    else if (type >= 0 || type == T_OBJECT) { v.kind = REFERENCE; v.view = type; }
    return v;
}
/* Normalize separators and . / .. components, so duplicate imports and
   cycles cannot be hidden by spelling the same path differently. */
static int path_join(char *out, char *parent, char *child)
{
    char raw[BASIC_PATH * 2], part[BASIC_PATH];
    int positions[64], count, pos, i, start, len, prefix, absolute;
    char sep, *last, *p;
    if (strlen(parent) >= BASIC_PATH || strlen(child) >= BASIC_PATH) return 0;
    sep = (strchr(parent, '\\') || strchr(child, '\\') ||
           (parent[0] && parent[1] == ':') || (child[0] && child[1] == ':')) ? '\\' : '/';
    absolute = child[0] == '/' || child[0] == '\\' || (child[0] && child[1] == ':');
    raw[0] = 0;
    if (!absolute) {
        last = 0;
        for (p = parent; *p; ++p) if (*p == '/' || *p == '\\') last = p;
        if (last) { len = (int)(last - parent + 1); memcpy(raw, parent, len); raw[len] = 0; }
    }
    strcat(raw, child);
    for (i = 0; raw[i]; ++i) if (raw[i] == '/' || raw[i] == '\\') raw[i] = sep;
    pos = prefix = count = i = 0;
    if (raw[0] && raw[1] == ':') { out[pos++] = raw[0]; out[pos++] = ':'; i = 2; }
    if (raw[i] == sep) out[pos++] = raw[i++];
    prefix = pos;
    while (raw[i]) {
        while (raw[i] == sep) ++i;
        start = i; while (raw[i] && raw[i] != sep) ++i;
        len = i - start; if (!len) break;
        memcpy(part, raw + start, len); part[len] = 0;
        if (!strcmp(part, ".")) continue;
        if (!strcmp(part, "..") && count && strcmp(out + positions[count - 1], "..")) {
            pos = positions[--count]; if (pos > prefix) --pos; out[pos] = 0; continue;
        }
        if (!strcmp(part, "..") && !count && prefix) return 0;
        if (pos && out[pos - 1] != sep && out[pos - 1] != ':') out[pos++] = sep;
        if (pos + len >= BASIC_PATH || count == 64) return 0;
        positions[count++] = pos; memcpy(out + pos, part, len); pos += len; out[pos] = 0;
    }
    out[pos] = 0; return pos != 0;
}
static int append(char *text, int file, int row)
{
    if (ncode == PROGRAM_LINES) { problem = "Project exceeds 2048 source lines"; return 0; }
    strcpy(code[ncode].text, text); code[ncode].file = file; code[ncode].row = row; ++ncode; return 1;
}
static int load_module(char *path, char source[MAX_LINES][LINE_LEN], int count, int origin);
static int load_line(char *text, int file, int row)
{
    char child[BASIC_PATH], full[BASIC_PATH];
    char *saved;
    int n;
    if (!append(text, file, row)) return 0;
    scan = code[ncode - 1].text; pc = ncode - 1;
    if (!word("IMPORTS")) return 1;
    space(); n = 0;
    if (*scan++ != '"') { fail("Imports expects a quoted BASIC filename"); return 0; }
    while (*scan && *scan != '"') {
        if (n == BASIC_PATH - 1) { fail("Import path too long"); return 0; }
        child[n++] = *scan++;
    }
    child[n] = 0;
    if (*scan != '"' || !n) { fail("Invalid Imports filename"); return 0; }
    ++scan; if (!endline()) return 0;
    if (!path_join(full, modules[file].path, child)) { fail("Invalid import path"); return 0; }
    saved = scan;
    if (!load_module(full, 0, 0, file == 0 ? row : modules[file].origin)) return 0;
    scan = saved; return 1;
}
static int load_module(char *path, char source[MAX_LINES][LINE_LEN], int count, int origin)
{
    FILE *f;
    char buf[LINE_LEN + 3];
    int i, file, row, len, caller_file, caller_row;
    caller_file = failure_file; caller_row = failure_row;
    if (pc >= 0 && pc < ncode) { caller_file = code[pc].file; caller_row = code[pc].row; }
    for (i = 0; i < nfiles; ++i) if (equal(path, modules[i].path)) {
        if (modules[i].state == 1) { fail("Circular Imports dependency"); return 0; }
        return 1;
    }
    if (nfiles == BASIC_FILES) { fail("Project exceeds 16 files"); return 0; }
    file = nfiles++; strcpy(modules[file].path, path);
    modules[file].state = 1; modules[file].origin = origin;
    f = 0;
    if (!source && file_reader) source = file_reader(path, &count);
    if (source) {
        if (count < 0 || count > MAX_LINES) { fail("Invalid cached source line count"); return 0; }
        for (i = 0; i < count; ++i) if (!memchr(source[i], 0, LINE_LEN)) {
            fail("Cached source line exceeds 159 characters"); return 0;
        }
    }
    if (!source) {
        f = fopen(path, "r");
        if (!f) { problem = "Cannot open imported file"; failure_file = file; failure_row = 1; return 0; }
    }
    row = 0;
    while (!problem) {
        if (source) {
            if (row >= count) break;
            strcpy(buf, source[row]);
        } else {
            if (!fgets(buf, sizeof(buf), f)) break;
            len = strlen(buf);
            while (len && (buf[len - 1] == '\r' || buf[len - 1] == '\n')) buf[--len] = 0;
            if (len >= LINE_LEN) {
                problem = "Source line exceeds 159 characters"; failure_file = file; failure_row = row + 1; break;
            }
        }
        if (++row > MAX_LINES) { problem = "File exceeds 256 source lines"; failure_file = file; failure_row = row; break; }
        failure_file = file; failure_row = row;
        if (!load_line(buf, file, row)) break;
    }
    if (f) {
        if (ferror(f) && !problem) { problem = "Error reading imported file"; failure_file = file; failure_row = row + 1; }
        fclose(f);
    }
    if (problem) return 0;
    modules[file].state = 2; failure_file = caller_file; failure_row = caller_row; return 1;
}
static int class_index(char *name)
{
    int i; for (i = 0; i < nclasses; ++i) if (!strcmp(name, classes[i].name)) return i;
    return -1;
}
static int own_method(int owner, char *name)
{
    int i; for (i = 0; i < nmethods; ++i) if (methods[i].owner == owner && !strcmp(name, methods[i].name)) return i;
    return -1;
}
static int method_index(int owner, char *name)
{
    int m, limit;
    if (owner < 0) return own_method(-1, name);
    for (limit = 0; owner >= 0 && limit < CLASSES; ++limit) {
        m = own_method(owner, name); if (m >= 0) return m; owner = classes[owner].base;
    }
    return -1;
}
static int field_index(int owner, char *name)
{
    int i;
    for (i = classes[owner].fields - 1; i >= 0; --i) if (!strcmp(name, classes[owner].field[i].name)) return i;
    return -1;
}
static int derives(int type, int base)
{
    int i;
    for (i = 0; type >= 0 && i < CLASSES; ++i) { if (type == base) return 1; type = classes[type].base; }
    return 0;
}
static int type_name(void)
{
    char name[NAME_LEN]; int type;
    if (!ident(name)) return T_NUMBER;
    if (!strcmp(name, "INTEGER") || !strcmp(name, "LONG")) return T_NUMBER;
    if (!strcmp(name, "STRING")) return T_STRING;
    if (!strcmp(name, "BOOLEAN")) return T_BOOLEAN;
    if (!strcmp(name, "VARIANT")) return T_ANY;
    if (!strcmp(name, "OBJECT")) return T_OBJECT;
    if (!strcmp(name, "COLLECTION")) return T_COLLECTION;
    type = class_index(name);
    if (type < 0) { fail("Unknown variable type"); return T_NUMBER; }
    return type;
}
static int check_type(int type, Value v)
{
    int actual;
    if (problem) return 0;
    if (type == T_ANY) return 1;
    if (type >= ARRAY_TYPE - 10) {
        if (v.kind == ARRAY && (v.object < 0 || containers[v.object].element_type == type - ARRAY_TYPE)) return 1;
        fail("Array element type does not match declaration"); return 0;
    }
    if (type == T_STRING) { if (v.kind == STRING) return 1; fail("Expected String value"); return 0; }
    if (type == T_COLLECTION) { if (v.kind == COLLECTION || (v.kind == REFERENCE && v.object < 0)) return 1; fail("Expected Collection value"); return 0; }
    if (type == T_NUMBER || type == T_BOOLEAN) {
        if (v.kind == NUMBER) return 1; fail("Numeric variable/field requires number"); return 0;
    }
    if (v.kind != REFERENCE) { fail("Object variable requires object or Nothing"); return 0; }
    if (v.object < 0 || type == T_OBJECT) return 1;
    actual = objects[v.object].type;
    if (derives(actual, type) || (classes[type].is_interface && (classes[actual].interfaces & (1UL << type)))) return 1;
    fail("Object Class does not match declaration"); return 0;
}
static int put(Value *target, int type, Value v)
{
    if (!check_type(type, v)) return 0;
    if (type == T_BOOLEAN) v.number = v.number ? -1 : 0;
    if (type == T_COLLECTION && v.kind == REFERENCE) v.kind = COLLECTION;
    if (v.kind == REFERENCE) v.view = type >= 0 ? type : -1;
    *target = v; return 1;
}
static int accessor(void) { return level ? methods[frames[level].method].owner : -1; }
static int allowed(int caller, int owner, int visibility)
{
    if (visibility == 1 || caller == owner || (visibility == 2 && caller >= 0 && derives(caller, owner))) return 1;
    fail("Private member is not accessible here"); return 0;
}
static void modifiers(int *visible, int *virt, int *override)
{
    *visible = 1; *virt = *override = 0;
    while (1) {
        if (word("PUBLIC")) *visible = 1;
        else if (word("PRIVATE")) *visible = 0;
        else if (word("PROTECTED")) *visible = 2;
        else if (word("OVERRIDABLE")) *virt = 1;
        else if (word("OVERRIDES")) { *override = 1; *virt = 1; }
        else break;
    }
}
static void class_headers(void)
{
    int visibility, virt, override, is_interface, i;
    char name[NAME_LEN];
    for (pc = 0; pc < ncode && !problem; ++pc) {
        scan = code[pc].text; modifiers(&visibility, &virt, &override);
        is_interface = word("INTERFACE");
        if (!is_interface && !word("CLASS")) continue;
        if (!declaration_name(name, 0)) break;
        if (class_index(name) >= 0) { fail("Duplicate Class name"); break; }
        if (nclasses == CLASSES) { fail("Maximum 16 classes/interfaces"); break; }
        i = nclasses++; memset(&classes[i], 0, sizeof(Class));
        strcpy(classes[i].name, name); classes[i].base = -1;
        classes[i].is_interface = is_interface; classes[i].declaration = pc;
        if (word("INHERITS")) ident(classes[i].base_name);
        endline();
    }
}
static void method_signature(int owner, int visible, int virt, int override, int function)
{
    Method *m; int index, i, array_parameter;
    char name[NAME_LEN];
    if (!declaration_name(name, owner >= 0)) return;
    if (own_method(owner, name) >= 0) { fail("Duplicate method or procedure"); return; }
    if (nmethods == METHODS) { fail("Maximum 64 methods/procedures"); return; }
    index = nmethods++; m = &methods[index]; memset(m, 0, sizeof(Method));
    strcpy(m->name, name); m->owner = owner; m->visible = visible;
    m->virtual_method = virt; m->overrides = override; m->function = function;
    m->result_type = function ? T_NUMBER : T_VOID; m->declaration = pc;
    m->start = pc + 1; m->end = -1;
    space(); if (*scan != '(') { fail("Expected parameter list (...)"); return; } ++scan; space();
    while (*scan != ')' && !problem) {
        i = m->argc;
        if (i == PARAMS) { fail("Maximum 8 parameters"); break; }
        if (!declaration_name(m->params[i], 0)) break;
        for (array_parameter = 0; array_parameter < i; ++array_parameter)
            if (!strcmp(m->params[array_parameter], m->params[i])) fail("Duplicate parameter");
        array_parameter = 0; space();
        if (*scan == '(') {
            ++scan; space(); if (*scan != ')') { fail("Array parameters use empty ()"); break; }
            ++scan; array_parameter = 1;
        }
        m->parameter_type[i] = word("AS") ? type_name() : T_NUMBER;
        if (array_parameter) m->parameter_type[i] += ARRAY_TYPE;
        ++m->argc; space(); if (*scan != ',') break; ++scan;
    }
    space(); if (*scan != ')') { fail("Expected closing parameter parenthesis"); return; } ++scan;
    if (function && word("AS")) m->result_type = type_name();
    space();
    if (function && *scan == '(') {
        ++scan; space(); if (*scan != ')') fail("Array result uses empty ()"); else ++scan;
        m->result_type += ARRAY_TYPE;
    }
    if (word("IMPLEMENTS")) {
        if (owner < 0 || classes[owner].is_interface) { fail("Method Implements requires a Class"); return; }
        if (!ident(m->contract)) return;
        space(); if (*scan != '.') { fail("Expected Interface.Member"); return; } ++scan;
        ident(m->contract_member);
    }
    if (!endline()) return;
    if (owner >= 0 && classes[owner].is_interface) {
        if (!strcmp(name, "NEW") || visible != 1) fail("Interface members must be public signatures");
        m->start = m->end = -1; return;
    }
    for (++pc; pc < ncode && !problem; ++pc) {
        if (code[pc].file != code[m->declaration].file) { fail("Method cannot cross file boundaries"); break; }
        scan = code[pc].text;
        if (word("END") && word(function ? "FUNCTION" : "SUB")) { if (endline()) m->end = pc; break; }
        scan = code[pc].text; modifiers(&visible, &virt, &override);
        if (word("SUB") || word("FUNCTION") || word("CLASS") || word("INTERFACE") || word("IMPORTS")) {
            fail("Nested definitions / Imports in method not supported"); break;
        }
    }
    if (m->end < 0 && !problem) { pc = m->declaration; fail("Missing End Sub / End Function"); }
    if (!problem) skip_to[m->declaration] = m->end + 1;
}
static Value constant(void)
{
    Value v; long n; int sign;
    v = number(0); space();
    if (*scan == '"') return string_literal();
    if (word("TRUE")) return number(-1);
    if (word("FALSE")) return v;
    if (word("NOTHING")) { v.kind = REFERENCE; return v; }
    sign = 1; if (*scan == '-' || *scan == '+') if (*scan++ == '-') sign = -1;
    space(); if (!isdigit((unsigned char)*scan)) { fail("Field default must be a literal"); return v; }
    n = 0;
    while (isdigit((unsigned char)*scan)) {
        n = n * 10 + (*scan++ - '0'); if (n > LIMIT) { fail("Number exceeds 1000000"); return v; }
    }
    return number(n * sign);
}
static void definitions(void)
{
    int owner, class_start, visible, virt, override, function, i, ending;
    char name[NAME_LEN]; Field *f;
    class_headers(); owner = class_start = -1;
    for (pc = 0; pc < ncode && !problem; ++pc) {
        scan = code[pc].text; space(); if (!*scan || *scan == '\'' || word("REM")) continue;
        if (word("IMPORTS")) { if (owner >= 0) fail("Imports must be outside a Class"); continue; }
        modifiers(&visible, &virt, &override);
        if (word("CLASS") || word("INTERFACE")) {
            if (owner >= 0) { fail("Nested Class is not supported"); break; }
            if (!ident(name)) break; owner = class_index(name); class_start = pc; continue;
        }
        if (word("END")) {
            ending = word("CLASS") ? 1 : word("INTERFACE") ? 2 : 0;
            if (ending) {
                if (owner < 0 || classes[owner].is_interface != (ending == 2)) { fail("Mismatched End Class / Interface"); break; }
                if (code[pc].file != code[class_start].file) { fail("Class cannot cross file boundaries"); break; }
                if (!endline()) break; skip_to[class_start] = pc + 1; owner = class_start = -1; continue;
            }
            scan = code[pc].text; modifiers(&visible, &virt, &override);
        }
        if (owner >= 0 && word("INHERITS")) {
            if (classes[owner].base_name[0]) { fail("Only one Inherits declaration allowed"); break; }
            if (ident(classes[owner].base_name)) endline(); continue;
        }
        if (owner >= 0 && word("IMPLEMENTS")) {
            if (classes[owner].is_interface) { fail("Interface uses Inherits, not Implements"); break; }
            do {
                i = classes[owner].contract_count;
                if (i == 4) { fail("Maximum 4 implemented interfaces per Class"); break; }
                if (!ident(classes[owner].contracts[i])) break;
                ++classes[owner].contract_count; space(); if (*scan != ',') break; ++scan;
            } while (!problem);
            endline(); continue;
        }
        function = word("FUNCTION");
        if (function || word("SUB")) { method_signature(owner, visible, virt, override, function); continue; }
        if (owner >= 0) {
            if (classes[owner].is_interface) { fail("Interface contains method signatures only"); break; }
            word("DIM"); if (!declaration_name(name, 0)) break;
            if (!word("AS")) { fail("Expected field As type"); break; }
            if (field_index(owner, name) >= 0 || own_method(owner, name) >= 0) { fail("Duplicate member name"); break; }
            if (classes[owner].fields == FIELDS) { fail("Maximum 16 fields per Class"); break; }
            f = &classes[owner].field[classes[owner].fields++];
            strcpy(f->name, name); f->type = type_name(); f->visible = visible; f->owner = owner;
            f->initial = default_value(f->type); space();
            if (*scan == '=') { ++scan; { Value initial; initial=constant(); put(&f->initial,f->type,initial); } }
            endline();
        }
    }
    if (!problem && owner >= 0) { pc = class_start; fail("Missing End Class / Interface"); }
}
static void resolve_class(int type)
{
    Class *c, *base; Field own[FIELDS]; int i, j, n, contract;
    c = &classes[type]; pc = c->declaration;
    if (c->resolved == 2) return;
    if (c->resolved == 1) { fail("Circular inheritance"); return; }
    c->resolved = 1;
    if (c->base_name[0]) {
        c->base = class_index(c->base_name);
        if (c->base < 0 || classes[c->base].is_interface != c->is_interface) { fail("Invalid base Class / Interface"); return; }
        resolve_class(c->base); if (problem) return; base = &classes[c->base]; pc = c->declaration;
        c->interfaces |= base->interfaces;
        if (c->is_interface) c->interfaces |= 1UL << c->base;
        n = c->fields; memcpy(own, c->field, sizeof(own));
        if (n + base->fields > FIELDS) { fail("Inherited fields exceed 16"); return; }
        memcpy(c->field, base->field, base->fields * sizeof(Field)); c->fields = base->fields;
        for (i = 0; i < n; ++i) {
            if (field_index(type, own[i].name) >= 0) { fail("Duplicate inherited field"); return; }
            c->field[c->fields++] = own[i];
        }
    }
    for (j = 0; j < c->contract_count && !problem; ++j) {
        contract = class_index(c->contracts[j]);
        if (contract < 0 || !classes[contract].is_interface) { pc = c->declaration; fail("Implements requires an Interface"); break; }
        resolve_class(contract); if (problem) break;
        c->interfaces |= (1UL << contract) | classes[contract].interfaces;
    }
    c->resolved = 2;
}
static int same_signature(int a, int b)
{
    int i;
    if (methods[a].argc != methods[b].argc || methods[a].result_type != methods[b].result_type) return 0;
    for (i = 0; i < methods[a].argc; ++i) if (methods[a].parameter_type[i] != methods[b].parameter_type[i]) return 0;
    return 1;
}
static int implementation(int type, int contract, char *name)
{
    int i, owner;
    for (owner = type; owner >= 0; owner = classes[owner].base)
        for (i = 0; i < nmethods; ++i)
            if (methods[i].owner == owner && methods[i].contract[0] &&
                class_index(methods[i].contract) == contract && !strcmp(methods[i].contract_member, name)) return i;
    return method_index(type, name);
}
static void validate_classes(void)
{
    int i, j, c, base, m, contract;
    for (i = 0; i < nclasses && !problem; ++i) resolve_class(i);
    for (i = 0; i < nmethods && !problem; ++i) {
        pc = methods[i].declaration; c = methods[i].owner;
        if (c < 0 || classes[c].is_interface) continue;
        if (field_index(c, methods[i].name) >= 0) { fail("Field and method have same name"); break; }
        base = classes[c].base;
        m = base < 0 ? -1 : method_index(base, methods[i].name);
        if (!strcmp(methods[i].name, "NEW")) {
            if (methods[i].function || methods[i].overrides) fail("Constructor must be Sub New");
        } else if (methods[i].overrides) {
            if (m < 0 || !methods[m].virtual_method || !same_signature(i, m) ||
                (methods[m].visible == 1 && methods[i].visible != 1)) fail("Invalid Overrides signature / base method");
        } else if (m >= 0 && methods[m].visible != 0) fail("Use Overrides for an inherited method");
        if (methods[i].contract[0]) {
            contract = class_index(methods[i].contract);
            if (contract < 0 || !(classes[c].interfaces & (1UL << contract))) { fail("Method Interface is not implemented by Class"); break; }
            m = method_index(contract, methods[i].contract_member);
            if (m < 0 || !same_signature(i, m) || methods[i].visible != 1) fail("Invalid interface implementation signature");
        }
    }
    for (c = 0; c < nclasses && !problem; ++c) if (!classes[c].is_interface) {
        pc = classes[c].declaration;
        for (j = 0; j < nclasses && !problem; ++j) if (classes[c].interfaces & (1UL << j)) {
            for (i = 0; i < nmethods && !problem; ++i) if (methods[i].owner == j) {
                m = implementation(c, j, methods[i].name);
                if (m < 0 || methods[m].visible != 1 || !same_signature(i, m)) fail("Class does not implement interface contract");
            }
        }
    }
}
static Symbol *find_symbol(char *name)
{
    int i, pass, frame;
    for (pass = 0; pass < 2; ++pass) {
        frame = pass ? 0 : level;
        for (i = 0; i < frames[frame].count; ++i)
            if (!strcmp(frames[frame].vars[i].name, name)) return &frames[frame].vars[i];
        if (!level) break;
    }
    return 0;
}
static Symbol *lookup(char *name, int create)
{
    Symbol *s; s = find_symbol(name); if (s) return s;
    if (!create) { fail("Unknown variable"); return 0; }
    if (basic_is_reserved(name)) { fail("Reserved word cannot be assigned"); return 0; }
    if (frames[level].count == SYMBOLS) { fail("Maximum 64 variables per scope"); return 0; }
    s = &frames[level].vars[frames[level].count++]; strcpy(s->name, name);
    s->type = T_NUMBER; s->declaration = -1; s->value = number(0); return s;
}
static Symbol *declare(char *name)
{
    int i; Symbol *s;
    for (i = 0; i < frames[level].count; ++i) if (!strcmp(name, frames[level].vars[i].name)) {
        if (frames[level].vars[i].declaration == pc) return &frames[level].vars[i];
        fail("Duplicate variable in this scope"); return 0;
    }
    if (frames[level].count == SYMBOLS) { fail("Maximum 64 variables per scope"); return 0; }
    s = &frames[level].vars[frames[level].count++]; memset(s, 0, sizeof(Symbol));
    strcpy(s->name, name); s->declaration = pc; return s;
}
static Value make_container(int kind, int element_type, long lower, long upper)
{
    Value v; Container *c; int i;
    v = number(0); v.kind = kind;
    if (lower < -LIMIT || upper > LIMIT || upper < lower - 1 || upper - lower + 1 > ITEMS) {
        fail("Container bounds exceed 128 items"); return v;
    }
    if (ncontainers == CONTAINERS) { fail("Maximum 32 arrays/collections per run"); return v; }
    v.object = ncontainers++; c = &containers[v.object]; c->kind = kind;
    c->lower = lower; c->count = (int)(upper - lower + 1); c->element_type = element_type; c->revision = 0;
    for (i = 0; i < c->count; ++i) c->item[i] = default_value(element_type);
    return v;
}
static Container *container(Value v)
{
    if ((v.kind != ARRAY && v.kind != COLLECTION) || v.object < 0 || v.object >= ncontainers) {
        fail("Array/Collection is not allocated"); return 0;
    }
    return &containers[v.object];
}
static int arguments(Value *args)
{
    int count; count = 0; space();
    if (*scan != '(') { fail("Expected ("); return 0; } ++scan; space();
    while (*scan != ')' && !problem) {
        if (count == PARAMS) { fail("Maximum 8 arguments"); break; }
        args[count++] = expression(); space(); if (*scan != ',') break; ++scan;
    }
    space(); if (*scan == ')') ++scan; else fail("Expected closing argument parenthesis");
    return count;
}
static Value self_value(int base)
{
    Value v; int owner;
    v = number(0); v.kind = REFERENCE; v.object = frames[level].self; owner = accessor();
    if (owner < 0 || v.object < 0) fail("Me is only valid inside a Class method");
    else { v.view = base ? classes[owner].base : owner; if (base && v.view < 0) fail("MyBase requires a base Class"); }
    return v;
}
static int object_type(Value v)
{
    if (v.kind != REFERENCE) { fail("Expected object reference"); return -1; }
    if (v.object < 0) { fail("Object is Nothing; use New first"); return -1; }
    return objects[v.object].type;
}
static Value construct(int type, Value *args, int argc)
{
    Value v; int i; v = number(0);
    if (classes[type].is_interface) { fail("Cannot instantiate an Interface"); return v; }
    if (nobjects == OBJECTS) { fail("Maximum 64 object instances per run"); return v; }
    v.kind = REFERENCE; v.object = nobjects++; v.view = type; objects[v.object].type = type;
    for (i = 0; i < classes[type].fields; ++i) objects[v.object].field[i] = classes[type].field[i].initial;
    initialize(type, v.object, args, argc, accessor()); return v;
}
static int comparison(Value a, Value b, char op, char next)
{
    int c;
    if (a.kind != b.kind) { fail("Cannot compare different value kinds"); return 0; }
    c = 0;
    if (a.kind == STRING) c = strcmp(a.string, b.string);
    else if (a.kind == NUMBER) c = a.number < b.number ? -1 : a.number > b.number ? 1 : 0;
    else if (op == '=' || (op == '<' && next == '>')) c = a.object == b.object ? 0 : 1;
    else { fail("References support equality comparisons only"); return 0; }
    if (op == '=') return c == 0;
    if (op == '<') return next == '=' ? c <= 0 : next == '>' ? c != 0 : c < 0;
    return next == '=' ? c >= 0 : c > 0;
}
static void format_value(Value v, char *text)
{
    if (v.kind == STRING) strcpy(text, v.string);
    else if (v.kind == NUMBER) sprintf(text, "%ld", v.number);
    else fail("Cannot print/convert an object or container");
}
static int parse_integer(char *text, long *result)
{
    char *p; long n; int sign;
    p = text; while (*p == ' ' || *p == '\t') ++p;
    sign = 1; if (*p == '-' || *p == '+') if (*p++ == '-') sign = -1;
    if (!isdigit((unsigned char)*p)) return 0;
    n = 0;
    while (isdigit((unsigned char)*p)) { n = n * 10 + (*p++ - '0'); if (n > LIMIT) return 0; }
    while (*p == ' ' || *p == '\t') ++p;
    if (*p) return 0; *result = n * sign; return 1;
}
static int builtin(char *name)
{
    return !strcmp(name,"LEN") || !strcmp(name,"STR") || !strcmp(name,"CSTR") || !strcmp(name,"VAL") ||
        !strcmp(name,"CINT") || !strcmp(name,"ABS") || !strcmp(name,"UBOUND") || !strcmp(name,"LBOUND") ||
        !strcmp(name,"COUNT") || !strcmp(name,"LEFT") || !strcmp(name,"RIGHT") || !strcmp(name,"MID") ||
        !strcmp(name,"UCASE") || !strcmp(name,"LCASE");
}
static Value builtin_call(char *name, Value *args, int argc)
{
    Value v; Container *c; char text[STRING_LEN]; long n, start, count; int i;
    v = number(0);
    if (!argc) { fail("Builtin requires arguments"); return v; }
    if (!strcmp(name,"LEN")) {
        if (argc != 1 || args[0].kind != STRING) fail("Len expects one String"); else v.number = strlen(args[0].string);
    } else if (!strcmp(name,"STR") || !strcmp(name,"CSTR")) {
        if (argc != 1) fail("CStr expects one value"); else { format_value(args[0],text); if (!problem) v = string_value(text); }
    } else if (!strcmp(name,"VAL") || !strcmp(name,"CINT")) {
        if (argc != 1) fail("CInt expects one value");
        else if (args[0].kind == NUMBER) v = args[0];
        else if (args[0].kind != STRING || !parse_integer(args[0].string,&n)) fail("Invalid integer text"); else v = number(n);
    } else if (!strcmp(name,"ABS")) {
        if (argc != 1) fail("Abs expects one number"); else { n = numeric(args[0]); v = number(n < 0 ? -n : n); }
    } else if (!strcmp(name,"UBOUND") || !strcmp(name,"LBOUND") || !strcmp(name,"COUNT")) {
        c = container(args[0]);
        if (argc != 1) fail("Bound/Count expects one container");
        else if (c) v = number(!strcmp(name,"COUNT") ? c->count : !strcmp(name,"LBOUND") ? c->lower : c->lower + c->count - 1);
    } else if (!strcmp(name,"UCASE") || !strcmp(name,"LCASE")) {
        if (argc != 1 || args[0].kind != STRING) fail("Case conversion expects one String");
        else { v = args[0]; for (i = 0; v.string[i]; ++i) v.string[i] = (char)(name[0]=='U' ? toupper((unsigned char)v.string[i]) : tolower((unsigned char)v.string[i])); }
    } else {
        if (args[0].kind != STRING || argc < 2 || argc > 3) { fail("Left/Right/Mid expects String and bounds"); return v; }
        n = strlen(args[0].string); start = 0; count = numeric(args[1]);
        if (!strcmp(name,"MID")) { start = count - 1; count = argc == 3 ? numeric(args[2]) : n; }
        else if (argc != 2) fail("Left/Right expects two arguments");
        else if (!strcmp(name,"RIGHT")) start = count >= n ? 0 : n - count;
        if (start < 0 || count < 0) { fail("Invalid String bounds"); return v; }
        if (start > n) start = n; if (count > n - start) count = n - start;
        memcpy(text,args[0].string+(int)start,(int)count); text[count] = 0; v = string_value(text);
    }
    return v;
}
/* A reference returns both its value and its assignable slot, if any. */
static Value reference(char *name, int create, int need_value, Value **slot, int *slot_type, int *called)
{
    Symbol *s; Value v, args[PARAMS]; Container *c;
    char member[NAME_LEN]; int m, argc, actual, view, f, base_dispatch, index, type; long raw_index;
    v = number(0); *slot = 0; *slot_type = T_ANY; *called = 0;
    base_dispatch = !strcmp(name,"MYBASE"); space(); s = find_symbol(name);
    if (!strcmp(name,"ME") || base_dispatch) v = self_value(base_dispatch);
    else if (!strcmp(name,"NOTHING")) { v.kind = REFERENCE; }
    else if (!strcmp(name,"TRUE")) v = number(-1);
    else if (!strcmp(name,"FALSE")) v = number(0);
    else if (*scan == '(' && (!s || (s->value.kind != ARRAY && s->value.kind != COLLECTION))) {
        argc = arguments(args); *called = 1;
        m = method_index(accessor(),name);
        if (m < 0) m = own_method(-1,name);
        if (m >= 0 && methods[m].owner >= 0 && methods[m].virtual_method && frames[level].self >= 0)
            m = method_index(objects[frames[level].self].type,name);
        if (m >= 0) {
            if (!problem && allowed(accessor(),methods[m].owner,methods[m].visible))
                v = invoke(m,methods[m].owner >= 0 ? frames[level].self : -1,args,argc,need_value);
        } else if (builtin(name)) v = builtin_call(name,args,argc);
        else fail("Unknown procedure or Function");
    } else {
        if (!s) s = lookup(name, create || (strlen(name)==1 && name[0]>='A' && name[0]<='Z'));
        if (s) { v = s->value; if (v.kind==REFERENCE && s->type>=0) v.view=s->type; *slot=&s->value; *slot_type=s->type; }
    }
    space();
    while (!problem && (*scan == '.' || (*scan == '(' && (v.kind == ARRAY || v.kind == COLLECTION)))) {
        if (*scan == '(') {
            c = container(v); argc = arguments(args);
            if (!c || problem) break;
            if (argc != 1) { fail("Indexing expects one subscript"); break; }
            raw_index = numeric(args[0]) - c->lower;
            if (raw_index < 0 || raw_index >= c->count) { fail("Container index out of bounds"); break; }
            index = (int)raw_index; *slot = &c->item[index]; *slot_type = c->element_type; v = **slot; *called = 0; space(); continue;
        }
        ++scan; if (!ident(member)) break; space();
        *slot = 0; *slot_type = T_ANY;
        if (v.kind == ARRAY || v.kind == COLLECTION) {
            c = container(v); if (!c) break;
            if (!strcmp(member,"COUNT") && *scan != '(') { v = number(c->count); space(); continue; }
            if (*scan != '(') { fail("Unknown container member"); break; }
            argc = arguments(args); *called = 1;
            if (!strcmp(member,"ITEM")) {
                if (argc != 1) { fail("Item expects one index"); break; }
                raw_index = numeric(args[0]) - c->lower;
                if (raw_index < 0 || raw_index >= c->count) { fail("Container index out of bounds"); break; }
                index = (int)raw_index; *slot = &c->item[index]; *slot_type = c->element_type; v = **slot;
            } else if (c->kind != COLLECTION) fail("Array supports Count and Item only");
            else if (!strcmp(member,"ADD")) {
                if (argc != 1) fail("Add expects one value");
                else if (c->count == ITEMS) fail("Collection exceeds 128 items");
                else { c->item[c->count++] = args[0]; ++c->revision; v = number(0); }
            } else if (!strcmp(member,"REMOVE")) {
                if (argc != 1) fail("Remove expects one index");
                else {
                    raw_index = numeric(args[0]) - c->lower;
                    if (raw_index < 0 || raw_index >= c->count) fail("Container index out of bounds");
                    else { index=(int)raw_index; memmove(c->item+index,c->item+index+1,(c->count-index-1)*sizeof(Value)); --c->count; ++c->revision; v=number(0); }
                }
            } else if (!strcmp(member,"CLEAR")) {
                if (argc) fail("Clear expects no arguments"); else { c->count=0; ++c->revision; v=number(0); }
            } else fail("Unknown Collection method");
            if (need_value && *called && strcmp(member,"ITEM")) fail("Collection mutation has no return value");
            space(); continue;
        }
        actual = object_type(v); if (problem) break; view = v.view >= 0 ? v.view : actual;
        if (*scan == '(') {
            argc = arguments(args); *called = 1;
            if (base_dispatch && !strcmp(member,"NEW")) {
                int first;
                first = methods[frames[level].method].start;
                while (first < pc) {
                    char *p; p = code[first].text; while (*p==' ' || *p=='\t') ++p;
                    if (*p && *p!='\'' && strncmp(p,"REM ",4)) break; ++first;
                }
                if (need_value || strcmp(methods[frames[level].method].name,"NEW") || pc != first) fail("MyBase.New must be first constructor statement");
                else initialize(view,v.object,args,argc,accessor());
                v = number(0);
            } else {
                m = method_index(view,member);
                if (m < 0) fail("Unknown object method");
                else if (allowed(accessor(),methods[m].owner,methods[m].visible)) {
                    if (classes[view].is_interface) m = implementation(actual,methods[m].owner,member);
                    else if (!base_dispatch && methods[m].virtual_method) m = method_index(actual,member);
                    if (m < 0) fail("Interface implementation not found");
                    else if (!problem) v = invoke(m,v.object,args,argc,need_value);
                }
            }
        } else {
            f = field_index(view,member);
            if (f < 0) fail("Unknown object field");
            else if (allowed(accessor(),classes[view].field[f].owner,classes[view].field[f].visible)) {
                *slot = &objects[v.object].field[f]; *slot_type = classes[view].field[f].type; v=**slot;
                if (v.kind==REFERENCE && *slot_type>=0) v.view=*slot_type;
            }
        }
        base_dispatch = 0; space();
    }
    type = *slot_type; if (v.kind == REFERENCE && type >= 0 && type < ARRAY_TYPE-10) v.view = type;
    return v;
}
static Value factor(void)
{
    Value v, args[PARAMS], *slot, element; Container *c;
    long n; int negative, unary, argc, type, called; char name[NAME_LEN];
    v=number(0); space();
    if (++expr_depth > 16) { fail("Expression nested too deeply"); --expr_depth; return v; }
    negative=unary=0;
    while (*scan=='+' || *scan=='-') { unary=1; if (*scan++=='-') negative=!negative; space(); }
    if (*scan=='"') v=string_literal();
    else if (*scan=='(') {
        ++scan; v=expression(); space(); if (*scan==')') ++scan; else fail("Missing closing parenthesis");
    } else if (isdigit((unsigned char)*scan)) {
        n=0;
        while (isdigit((unsigned char)*scan)) { n=n*10+(*scan++-'0'); if (n>LIMIT) { fail("Number exceeds 1000000"); n=0; break; } }
        v=number(n);
    } else if (word("TYPEOF")) {
        element=factor(); if (!word("IS") || !ident(name)) fail("Expected TypeOf object Is type");
        else {
            type=class_index(name); if (type<0) fail("Unknown Class / Interface");
            else if (element.kind!=REFERENCE) fail("TypeOf expects an object");
            else v=number(element.object>=0 && (derives(objects[element.object].type,type) ||
                (classes[type].is_interface && (classes[objects[element.object].type].interfaces & (1UL<<type)))) ? -1 : 0);
        }
    } else if (word("NEW")) {
        if (ident(name)) {
            argc=0; space(); if (*scan=='(') argc=arguments(args);
            if (!strcmp(name,"COLLECTION")) {
                if (argc) fail("New Collection expects no arguments"); else v=make_container(COLLECTION,T_ANY,1,0);
            } else { type=class_index(name); if (type<0) fail("Unknown Class after New"); else if (!problem) v=construct(type,args,argc); }
        }
    } else if (word("ARRAY")) {
        v=make_container(ARRAY,T_ANY,0,-1); c=container(v); space();
        if (*scan!='(') fail("Array expects (...)"); else ++scan;
        space();
        while (!problem && *scan!=')') {
            element=expression();
            if (c->count==ITEMS) fail("Array exceeds 128 items"); else c->item[c->count++]=element;
            space(); if (*scan!=',') break; ++scan;
        }
        space(); if (*scan==')') ++scan; else fail("Expected closing Array parenthesis");
    } else if (ident(name)) v=reference(name,0,1,&slot,&type,&called);
    if (unary) v=number(negative ? -numeric(v) : numeric(v));
    --expr_depth; return v;
}
static Value product(void)
{
    Value a,b; long x,y; int mod; char op;
    a=factor(); space();
    while (!problem) {
        mod=word("MOD");
        if (!mod && *scan!='*' && *scan!='/') break;
        op=mod ? '%' : *scan++; b=factor(); x=numeric(a); y=numeric(b);
        if (problem) break;
        if (op=='*') {
            if (y && (x<0?-x:x)>LIMIT/(y<0?-y:y)) fail("Result exceeds numeric range"); else x*=y;
        } else if (!y) fail("Division by zero"); else if (op=='%') x%=y; else x/=y;
        a=number(x); space();
    }
    return a;
}
static Value sum(void)
{
    Value a,b; long x,y; char op;
    a=product(); space();
    while (!problem && (*scan=='+' || *scan=='-' || *scan=='&')) {
        op=*scan++; b=product();
        if ((op=='+' || op=='&') && a.kind==STRING && b.kind==STRING) {
            if (strlen(a.string)+strlen(b.string)>=STRING_LEN) fail("String exceeds 127 characters"); else strcat(a.string,b.string);
        } else {
            if (op=='&') { fail("String concatenation expects Strings"); break; }
            x=numeric(a); y=numeric(b); x=op=='+' ? x+y : x-y;
            if (x>LIMIT || x< -LIMIT) fail("Result exceeds numeric range"); a=number(x);
        }
        space();
    }
    return a;
}
static Value compare_expression(void)
{
    Value a,b; char op,next; int negate;
    a=sum(); space();
    if (!problem && (word("ISNOT") || word("IS"))) {
        negate=toupper((unsigned char)scan[-1])=='T';
        if (!negate && word("NOT")) negate=1;
        b=sum();
        if (a.kind!=REFERENCE || b.kind!=REFERENCE) fail("Is expects object references");
        else a=number((a.object==b.object)!=negate ? -1 : 0);
    } else if (!problem && (*scan=='=' || *scan=='<' || *scan=='>')) {
        op=*scan++; next=0; if (*scan=='=' || (op=='<' && *scan=='>')) next=*scan++;
        b=sum(); a=number(comparison(a,b,op,next) ? -1 : 0);
    }
    return a;
}
static Value logical_not(void)
{
    Value v;
    if (word("NOT")) { if (++expr_depth>16) { fail("Expression nested too deeply"); --expr_depth; return number(0); }
        v=logical_not(); --expr_depth; return number(numeric(v) ? 0 : -1); }
    return compare_expression();
}
static Value logical_and(void)
{
    Value a,b; a=logical_not();
    while (!problem && word("AND")) { b=logical_not(); a=number(numeric(a) && numeric(b) ? -1 : 0); }
    return a;
}
static Value expression(void)
{
    Value a,b; int xor_op; a=logical_and();
    while (!problem) {
        xor_op=word("XOR"); if (!xor_op && !word("OR")) break;
        b=logical_and(); a=number(xor_op ? ((numeric(a)!=0)!=(numeric(b)!=0) ? -1 : 0) : (numeric(a)||numeric(b) ? -1 : 0));
    }
    return a;
}
static int blank_line(int line)
{
    char *saved; int result;
    saved=scan; scan=code[line].text; space(); result=!*scan || *scan=='\'' || word("REM"); scan=saved; return result;
}
static char *find_keyword(char *text,char *keyword)
{
    char *p; int quote,i;
    quote=0;
    for (p=text; *p; ++p) {
        if (*p=='"') {
            if (quote && p[1]=='"') { ++p; continue; }
            quote=!quote; continue;
        }
        if (!quote && *p=='\'') break;
        if (quote || (p>text && (isalnum((unsigned char)p[-1]) || p[-1]=='_'))) continue;
        for (i=0; keyword[i]; ++i) if (toupper((unsigned char)p[i])!=keyword[i]) break;
        if (!keyword[i] && !isalnum((unsigned char)p[i]) && p[i]!='_') return p;
    }
    return 0;
}
static void build_flow(int start, int end)
{
    int stack[BLOCKS], kind[BLOCKS], previous[BLOCKS], had_else[BLOCKS];
    int depth,i,k,open,close,inline_if; char *p;
    depth=0;
    for (i=start; i<end && !problem; ++i) {
        pc=i; if (blank_line(i)) continue;
        if (skip_to[i]) {
            if (depth) fail("Definition cannot be inside control flow");
            i=skip_to[i]-1; continue;
        }
        scan=code[i].text; open=close=0;
        if (word("IF")) {
            /* Find Then outside strings. An inline body does not open a block. */
            p=find_keyword(scan,"THEN");
            if (!p) { fail("If expects condition Then"); break; }
            scan=p+4; space(); inline_if=*scan && *scan!='\'';
            if (inline_if) block_end[i]=-1; else open=B_IF;
        } else if (word("ELSEIF") || word("ELSE")) {
            if (!depth || kind[depth-1]!=B_IF || had_else[depth-1]) { fail("Else/ElseIf without matching If"); break; }
            branch_next[previous[depth-1]]=i; previous[depth-1]=i;
            scan=code[i].text; if (word("ELSE")) { had_else[depth-1]=1; endline(); }
        } else if (word("WHILE")) open=B_WHILE;
        else if (word("FOR")) open=B_FOR;
        else if (word("DO")) open=B_DO;
        else if (word("SELECT")) { if (!word("CASE")) fail("Expected Select Case"); open=B_SELECT; }
        else if (word("CASE")) {
            if (!depth || kind[depth-1]!=B_SELECT || had_else[depth-1]) { fail("Case without matching Select"); break; }
            branch_next[previous[depth-1]]=i; previous[depth-1]=i;
            if (word("ELSE")) { had_else[depth-1]=1; endline(); }
        } else if (word("WEND")) { close=B_WHILE; endline(); }
        else if (word("NEXT")) close=B_FOR;
        else if (word("LOOP")) close=B_DO;
        else if (word("END")) {
            if (word("IF")) close=B_IF;
            else if (word("WHILE")) close=B_WHILE;
            else if (word("SELECT")) close=B_SELECT;
            if (close) endline();
        } else if (depth && kind[depth-1]==B_SELECT && previous[depth-1]==stack[depth-1]) fail("Select requires a Case before statements");
        if (open) {
            if (depth==BLOCKS) { fail("Control flow nesting exceeds 32"); break; }
            stack[depth]=i; kind[depth]=open; previous[depth]=i; had_else[depth]=0; ++depth;
        }
        if (close) {
            if (!depth || kind[depth-1]!=close) { fail("Mismatched control-flow terminator"); break; }
            --depth; k=stack[depth]; block_end[k]=i;
            branch_next[previous[depth]]=i;
            while (k!=i) { block_end[k]=i; k=branch_next[k]; }
            block_end[i]=stack[depth];
            if (code[i].file!=code[stack[depth]].file) fail("Block cannot cross file boundaries");
        }
    }
    if (!problem && depth) { pc=stack[depth-1]; fail("Missing control-flow terminator"); }
}
static void print_line(void)
{
    Value v; char text[OUT_LEN], item[STRING_LEN]; int len,n;
    text[0]=0; len=0;
    while (!problem) {
        space(); if (!*scan || *scan=='\'') break;
        v=expression(); format_value(v,item); if (problem) break; n=strlen(item);
        if (len+n>=OUT_LEN) { fail("PRINT output exceeds 79 characters"); break; }
        strcat(text,item); len+=n; space(); if (*scan!=';') break; ++scan;
    }
    if (endline() && !problem) emit(text);
}
static void read_input(void)
{
    char prompt[STRING_LEN], buffer[STRING_LEN], name[NAME_LEN];
    Value *slot,v; int type,called,ok; long n;
    strcpy(prompt,"Input"); space();
    if (*scan=='"') {
        v=string_literal(); strcpy(prompt,v.string); space();
        if (*scan!=';' && *scan!=',') { fail("Input prompt must be followed by ;"); return; } ++scan;
    }
    if (!ident(name)) return;
    v=reference(name,1,1,&slot,&type,&called);
    if (!slot || called || !endline()) { if (!problem) fail("Input requires an assignable variable"); return; }
    if (type!=T_STRING && type!=T_NUMBER && type!=T_BOOLEAN && type!=T_ANY) { fail("Input expects String or numeric target"); return; }
    buffer[0]=0;
    if (input_reader) ok=input_reader(prompt,buffer,sizeof(buffer));
    else { printf("%s ",prompt); fflush(stdout); ok=fgets(buffer,sizeof(buffer),stdin)!=0; }
    buffer[STRING_LEN-1]=0;
    if (!ok) { fail("Input cancelled"); return; }
    n=strlen(buffer); while (n && (buffer[n-1]=='\r' || buffer[n-1]=='\n')) buffer[--n]=0;
    if (type==T_STRING || (type==T_ANY && (v.kind==STRING || !parse_integer(buffer,&n)))) v=string_value(buffer);
    else if (!parse_integer(buffer,&n)) { fail("Invalid numeric Input"); return; } else v=number(n);
    put(slot,type,v);
}
static void bounds(long *lower, long *upper, int *dynamic)
{
    space(); *dynamic=0; *lower=0; *upper=-1;
    if (*scan!='(') { fail("Expected array bounds (...)"); return; } ++scan; space();
    if (*scan==')') { ++scan; *dynamic=1; return; }
    *upper=numeric(expression());
    if (word("TO")) { *lower=*upper; *upper=numeric(expression()); }
    space(); if (*scan!=')') fail("Expected closing bounds parenthesis"); else ++scan;
}
static void simple(char *text)
{
    char name[NAME_LEN]; Symbol *s; Value v,*slot,old; Container *c,*previous;
    int type,called,as_new,array,dynamic,preserve,i; long lower,upper;
    scan=text; expr_depth=0; space();
    if (!*scan || *scan=='\'' || word("REM") || word("IMPORTS")) return;
    if (word("PRINT")) { print_line(); return; }
    if (word("INPUT")) { read_input(); return; }
    if (word("RETURN")) {
        if (!level) { fail("Return is only valid inside a method"); return; }
        if (methods[frames[level].method].function) {
            v=expression(); put(&return_value,methods[frames[level].method].result_type,v);
        } else return_value=number(0);
        if (endline()) returned=1; return;
    }
    preserve=word("CONTINUE");
    if (preserve || word("EXIT")) {
        type=word("FOR") ? B_FOR : word("WHILE") ? B_WHILE : word("DO") ? B_DO : word("SELECT") ? B_SELECT : 0;
        if (type) {
            if (preserve && type==B_SELECT) fail("Continue Select is not valid");
            else if (endline()) { signal_kind=type; signal_continue=preserve; }
        } else if (!preserve && level && word(methods[frames[level].method].function ? "FUNCTION" : "SUB")) {
            if (methods[frames[level].method].function) fail("Function must Return a value"); else if (endline()) returned=1;
        } else fail("Expected Exit/Continue with matching block kind");
        return;
    }
    if (word("END")) {
        if (level) fail("Use Return or Exit Sub inside a method"); else if (endline()) stopped=1; return;
    }
    if (word("REDIM")) {
        preserve=word("PRESERVE"); if (!ident(name)) return; s=lookup(name,0); if (!s) return;
        if (s->type<ARRAY_TYPE-10) { fail("ReDim requires an array variable"); return; }
        bounds(&lower,&upper,&dynamic); if (!endline() || problem) return;
        if (dynamic) { fail("ReDim requires bounds"); return; }
        old=s->value; v=make_container(ARRAY,s->type-ARRAY_TYPE,lower,upper);
        if (preserve && old.object>=0 && !problem) {
            c=container(v); previous=container(old);
            if (c->lower!=previous->lower) fail("ReDim Preserve cannot change lower bound");
            else for (i=0; i<c->count && i<previous->count; ++i) c->item[i]=previous->item[i];
        }
        if (!problem) s->value=v; return;
    }
    if (word("DIM")) {
        if (!declaration_name(name,0)) return; s=declare(name); if (!s) return;
        array=0; dynamic=0; lower=0; upper=-1; space();
        if (*scan=='(') { array=1; bounds(&lower,&upper,&dynamic); }
        if (!word("AS")) { fail("Expected Dim name As type"); return; }
        as_new=word("NEW");
        if (as_new) {
            char *initial; initial=scan-3; type=type_name(); s->type=type; s->value=default_value(type);
            scan=initial; v=expression(); if (endline()) put(&s->value,s->type,v);
        } else {
            type=type_name(); s->type=array ? type+ARRAY_TYPE : type; s->value=default_value(s->type);
            if (array && !dynamic && !problem) s->value=make_container(ARRAY,type,lower,upper);
            space(); if (*scan=='=') { ++scan; v=expression(); put(&s->value,s->type,v); }
            endline();
        }
        return;
    }
    preserve=word("CALL"); if (!preserve) word("LET");
    if (!ident(name)) return;
    v=reference(name,!preserve,0,&slot,&type,&called); space();
    if (*scan=='=' && !preserve) {
        if (!slot) { fail("Expression is not assignable"); return; } ++scan;
        v=expression(); if (endline()) put(slot,type,v);
    } else if (!called || !endline()) { if (!problem) fail("Expected assignment or method call"); }
}
static int handle_signal(int kind)
{
    int result;
    if (!signal_kind) return 0;
    if (signal_kind!=kind) return 2;
    result=signal_continue ? 0 : 1; signal_kind=signal_continue=0; return result;
}
static void inline_if(void)
{
    Value condition; char body[LINE_LEN], *p,*branch; int len;
    word("IF"); condition=expression();
    if (!word("THEN")) { fail("If expects Then"); return; }
    space(); p=scan; branch=find_keyword(p,"ELSE");
    if (numeric(condition)) { len=branch ? (int)(branch-p) : (int)strlen(p); memcpy(body,p,len); body[len]=0; }
    else if (branch) strcpy(body,branch+4); else body[0]=0;
    if (!problem && body[0]) simple(body);
}
static void execute_if(int start,int end)
{
    int branch,next,take; Value v;
    branch=start;
    while (branch<end && !problem) {
        pc=branch; scan=code[branch].text;
        if (word("IF") || word("ELSEIF")) {
            v=expression(); take=numeric(v)!=0;
            if (!word("THEN") || !endline()) { if (!problem) fail("Expected Then"); return; }
        } else { word("ELSE"); take=endline(); }
        next=branch_next[branch];
        if (take && !problem) { run_range(branch+1,next); return; }
        branch=next;
    }
}
static int case_matches(Value selector)
{
    Value a,b; char op,next; int result,matched;
    if (word("ELSE")) return endline();
    matched=0;
    do {
        result=0;
        if (word("IS")) {
            space(); op=*scan; next=0;
            if (op!='=' && op!='<' && op!='>') { fail("Case Is expects comparison operator"); return 0; } ++scan;
            if (*scan=='=' || (op=='<' && *scan=='>')) next=*scan++;
            a=expression(); result=comparison(selector,a,op,next);
        } else {
            a=expression();
            if (word("TO")) { b=expression(); result=comparison(selector,a,'>', '=') && comparison(selector,b,'<','='); }
            else result=comparison(selector,a,'=',0);
        }
        if (result) matched=1; space(); if (*scan!=',') break; ++scan;
    } while (!problem);
    endline(); return matched;
}
static void execute_select(int start,int end)
{
    Value selector; int branch,next;
    pc=start; scan=code[start].text; word("SELECT"); word("CASE"); selector=expression(); if (!endline()) return;
    branch=branch_next[start];
    while (branch<end && !problem) {
        pc=branch; scan=code[branch].text; word("CASE"); next=branch_next[branch];
        if (case_matches(selector)) { run_range(branch+1,next); handle_signal(B_SELECT); return; }
        branch=next;
    }
}
static void execute_while(int start,int end)
{
    Value condition; int action;
    while (!problem && !returned && !stopped && tick()) {
        pc=start; scan=code[start].text; word("WHILE"); condition=expression(); if (!endline() || !numeric(condition)) break;
        run_range(start+1,end); action=handle_signal(B_WHILE); if (action) break;
    }
}
static void execute_do(int start,int end)
{
    int pre,post,until,action; Value condition;
    pc=start; scan=code[start].text; word("DO"); pre=word("WHILE"); until=0;
    if (!pre && word("UNTIL")) { pre=1; until=1; }
    pc=end; scan=code[end].text; word("LOOP"); post=word("WHILE") || word("UNTIL");
    if (pre && post) { fail("Do cannot have both pre and post conditions"); return; }
    while (!problem && !returned && !stopped && tick()) {
        pc=start; scan=code[start].text; word("DO");
        if (pre) {
            if (until) word("UNTIL"); else word("WHILE");
            condition=expression(); if (!endline() || ((numeric(condition)!=0)==until)) break;
        } else if (!endline()) break;
        run_range(start+1,end); action=handle_signal(B_DO); if (action || problem || returned || stopped) break;
        pc=end; scan=code[end].text; word("LOOP");
        if (post) {
            until=word("UNTIL"); if (!until) word("WHILE");
            condition=expression(); if (!endline() || ((numeric(condition)!=0)==until)) break;
        } else endline();
    }
}
static void execute_for(int start,int end)
{
    Symbol *s; Value initial,finish,step,sequence; Container *c;
    char name[NAME_LEN],closing[NAME_LEN]; long n,limit,delta; int each,type,i,action; long revision;
    pc=start; scan=code[start].text; word("FOR"); each=word("EACH");
    if (!declaration_name(name,0)) return;
    if (word("AS")) {
        type=type_name(); s=declare(name); if (!s) return; s->type=type; s->value=default_value(type);
    } else {
        s=find_symbol(name);
        if (!s) { s=lookup(name,1); if (s && each) s->type=T_ANY; }
        if (!s) return;
    }
    if (each) {
        if (!word("IN")) { fail("For Each expects In"); return; }
        sequence=expression(); c=container(sequence); if (!c || !endline()) return; revision=c->revision;
        pc=end; scan=code[end].text; word("NEXT"); space();
        if (*scan && *scan!='\'') { if (!ident(closing) || strcmp(name,closing)) fail("Next variable does not match For"); }
        endline();
        for (i=0; i<c->count && !problem && !returned && !stopped && tick(); ++i) {
            pc=start;
            if (c->revision!=revision) { fail("Collection changed during For Each"); break; }
            if (!put(&s->value,s->type,c->item[i])) break;
            run_range(start+1,end); action=handle_signal(B_FOR); if (action) break;
            if (c->revision!=revision) { pc=start; fail("Collection changed during For Each"); break; }
        }
    } else {
        if (s->type!=T_NUMBER && s->type!=T_ANY) { fail("Numeric For requires Integer/Long counter"); return; }
        space(); if (*scan!='=') { fail("For expects = start To end"); return; } ++scan;
        initial=expression(); if (!word("TO")) { fail("For expects To"); return; }
        finish=expression(); step=word("STEP") ? expression() : number(1);
        n=numeric(initial); limit=numeric(finish); delta=numeric(step);
        if (!endline() || problem) return;
        if (!delta) { fail("For Step cannot be zero"); return; }
        if (!put(&s->value,s->type,initial)) return;
        pc=end; scan=code[end].text; word("NEXT"); space();
        if (*scan && *scan!='\'') { if (!ident(closing) || strcmp(name,closing)) fail("Next variable does not match For"); }
        endline();
        while (!problem && !returned && !stopped && (delta>0 ? n<=limit : n>=limit) && tick()) {
            run_range(start+1,end); action=handle_signal(B_FOR); if (action || problem || returned || stopped) break;
            pc=start; n=numeric(s->value)+delta;
            if (n>LIMIT || n< -LIMIT) { fail("For counter exceeds numeric range"); break; }
            put(&s->value,s->type,number(n));
        }
    }
}
static void run_range(int start,int end)
{
    int current,next;
    for (current=start; current<end && !problem && !returned && !stopped && !signal_kind; current=next) {
        pc=current; next=current+1;
        if (skip_to[current]) { next=skip_to[current]; continue; }
        if (!tick()) break;
        scan=code[current].text; expr_depth=0;
        if (word("IF")) {
            scan=code[current].text;
            if (block_end[current]<0) inline_if(); else { execute_if(current,block_end[current]); next=block_end[current]+1; }
        } else if (word("WHILE")) { execute_while(current,block_end[current]); next=block_end[current]+1; }
        else if (word("FOR")) { execute_for(current,block_end[current]); next=block_end[current]+1; }
        else if (word("DO")) { execute_do(current,block_end[current]); next=block_end[current]+1; }
        else if (word("SELECT")) { execute_select(current,block_end[current]); next=block_end[current]+1; }
        else simple(code[current].text);
    }
}
static Value invoke(int m,int self,Value *args,int argc,int need_value)
{
    char *saved_scan; int saved_pc,saved_depth,saved_returned,saved_signal,saved_continue,i;
    Value saved_return,result; Method *method; Frame *frame;
    method=&methods[m]; result=default_value(method->result_type);
    if (argc!=method->argc) { fail("Wrong number of method arguments"); return result; }
    if (need_value && !method->function) { fail("Sub cannot be used as an expression"); return result; }
    if (level==CALL_DEPTH) { fail("Maximum method call depth is 8"); return result; }
    if (method->start<0) { fail("Cannot execute Interface signature"); return result; }
    for (i=0; i<argc && !problem; ++i) check_type(method->parameter_type[i],args[i]);
    if (problem) return result;
    saved_scan=scan; saved_pc=pc; saved_depth=expr_depth; saved_returned=returned; saved_return=return_value;
    saved_signal=signal_kind; saved_continue=signal_continue;
    frame=&frames[++level]; frame->self=self; frame->method=m; frame->count=argc;
    for (i=0; i<argc; ++i) {
        strcpy(frame->vars[i].name,method->params[i]); frame->vars[i].type=method->parameter_type[i]; frame->vars[i].declaration=-2;
        put(&frame->vars[i].value,frame->vars[i].type,args[i]);
    }
    returned=signal_kind=signal_continue=0; return_value=default_value(method->result_type);
    run_range(method->start,method->end);
    if (!problem && signal_kind) fail("Exit/Continue has no matching block in method");
    if (!problem && method->function && !returned) { pc=method->end; fail("Function finished without Return"); }
    if (!problem) result=return_value;
    --level; scan=saved_scan; pc=saved_pc; expr_depth=saved_depth; returned=saved_returned; return_value=saved_return;
    signal_kind=saved_signal; signal_continue=saved_continue; return result;
}
static void initialize(int type,int object,Value *args,int argc,int caller)
{
    int m,base,first,explicit_base; char *saved_scan;
    m=own_method(type,"NEW"); base=classes[type].base; explicit_base=0;
    if (m>=0) {
        if (!allowed(caller,type,methods[m].visible)) return;
        first=methods[m].start; while (first<methods[m].end && blank_line(first)) ++first;
        if (first<methods[m].end) {
            saved_scan=scan; scan=code[first].text; word("CALL"); explicit_base=word("MYBASE"); space();
            if (explicit_base && *scan=='.') { ++scan; explicit_base=word("NEW"); } else explicit_base=0;
            scan=saved_scan;
        }
    } else if (argc) { fail("Class has no constructor accepting arguments"); return; }
    if (base>=0 && !explicit_base) initialize(base,object,0,0,type);
    if (!problem && m>=0) invoke(m,object,args,argc,0);
}
int basic_file_count(void) { return nfiles; }
char *basic_file_path(int file) { return file >= 0 && file < nfiles ? modules[file].path : ""; }
char *basic_file_name(int file)
{
    char *name, *p;
    name = basic_file_path(file);
    for (p = name; *p; ++p) if (*p == '/' || *p == '\\') name = p + 1;
    return name;
}
char *basic_error_path(void) { return basic_file_path(failure_file); }
int basic_error_main_line(void)
{
    if (!problem) return 0;
    return failure_file == 0 ? failure_row : modules[failure_file].origin;
}
int basic_scan_project(char source[MAX_LINES][LINE_LEN], int count, char *mainpath)
{
    char path[BASIC_PATH];
    int i;
    ncode = nfiles = 0; problem = 0; pc = -1; failure_file = 0; failure_row = 1;
    if (count < 0 || count > MAX_LINES) return 0;
    for (i = 0; i < count; ++i) if (!memchr(source[i], 0, LINE_LEN)) return 0;
    if (!path_join(path, "", mainpath)) return 0;
    return load_module(path, source, count, 0);
}
int basic_run_project(char source[MAX_LINES][LINE_LEN],int count,char *mainpath,char output[OUT_LINES][OUT_LEN],int *used)
{
    char path[BASIC_PATH],text[OUT_LEN]; int i;
    sink=output; sink_used=used; *used=0;
    ncode=nfiles=nclasses=nmethods=nobjects=ncontainers=level=returned=stopped=signal_kind=signal_continue=0;
    steps=0; problem=0; pc=-1; failure_file=0; failure_row=1;
    memset(frames,0,sizeof(frames)); memset(skip_to,0,sizeof(skip_to));
    memset(block_end,0,sizeof(block_end)); memset(branch_next,0,sizeof(branch_next));
    frames[0].self=frames[0].method=-1;
    if (count<0 || count>MAX_LINES) problem="Invalid main source line count";
    for (i=0; i<count && !problem; ++i) if (!memchr(source[i],0,LINE_LEN)) problem="Source line exceeds 159 characters";
    if (!problem && !path_join(path,"",mainpath)) problem="Invalid main source path";
    if (!problem) load_module(path,source,count,0);
    if (!problem) definitions();
    if (!problem) validate_classes();
    if (!problem) build_flow(0,ncode);
    for (i=0; i<nmethods && !problem; ++i) if (methods[i].start>=0) build_flow(methods[i].start,methods[i].end);
    if (!problem) run_range(0,ncode);
    if (!problem && signal_kind) fail("Exit/Continue has no matching block");
    if (problem) { sprintf(text,"%.24s:%d: %.44s",basic_file_name(failure_file),failure_row,problem); emit(text); return failure_row; }
    emit("Program finished."); return 0;
}
int basic_run(char source[MAX_LINES][LINE_LEN],int count,char output[OUT_LINES][OUT_LEN],int *used)
{
    return basic_run_project(source,count,"MAIN.BAS",output,used);
}
