/* TT BASIC 0.3: bounded, case-insensitive BASIC with file imports and objects.
   ANSI C / Pure C. All program storage is static; recursion is bounded. */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "BASIC.H"
#define LIMIT 1000000L
#define PROGRAM_LINES 512
#define NAME_LEN 24
#define CLASSES 8
#define FIELDS 8
#define METHODS 32
#define PARAMS 4
#define OBJECTS 32
#define SYMBOLS 48
#define CALL_DEPTH 8
#define NUMBER 0
#define REFERENCE 1
typedef struct { char text[LINE_LEN]; int file, row; } Code;
typedef struct { char path[BASIC_PATH]; int state, origin; } Module;
typedef struct { int kind, object; long number; } Value;
typedef struct { char name[NAME_LEN]; int type; Value value; } Symbol;
typedef struct { char name[NAME_LEN]; int visible; long initial; } Field;
typedef struct { char name[NAME_LEN]; int fields; Field field[FIELDS]; } Class;
typedef struct {
    char name[NAME_LEN], params[PARAMS][NAME_LEN];
    int owner, visible, function, argc, start, end;
} Method;
typedef struct { int type; Value field[FIELDS]; } Instance;
typedef struct { int self, method, count; Symbol vars[SYMBOLS]; } Frame;
static Code code[PROGRAM_LINES];
static Module modules[BASIC_FILES];
static Class classes[CLASSES];
static Method methods[METHODS];
static Instance objects[OBJECTS];
static Frame frames[CALL_DEPTH + 1];
static int skip_to[PROGRAM_LINES];
static int ncode, nfiles, nclasses, nmethods, nobjects, level;
static int pc, failure_file, failure_row, expr_depth;
static int returned, stopped;
static Value return_value;
static char *scan, *problem;
static char (*sink)[OUT_LEN];
static int *sink_used;
static BasicReader file_reader;
void basic_set_reader(BasicReader reader) { file_reader = reader; }
static Value expression(void);
static Value invoke(int method, int self, Value *args, int argc, int need_value);
static void statement(void);
static Value number(long n)
{
    Value v; v.kind = NUMBER; v.number = n; v.object = -1; return v;
}
static void fail(char *message)
{
    if (problem) return;
    problem = message;
    if (pc >= 0 && pc < ncode) {
        failure_file = code[pc].file; failure_row = code[pc].row;
    }
}
static void space(void) { while (*scan == ' ' || *scan == '\t') ++scan; }
static int equal(char *a, char *b)
{
    while (*a && *b) {
        if (toupper((unsigned char)*a++) != toupper((unsigned char)*b++)) return 0;
    }
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
static int ident(char *name)
{
    int n;
    space(); n = 0;
    if (!isalpha((unsigned char)*scan) && *scan != '_') { fail("Expected identifier"); return 0; }
    while (isalnum((unsigned char)*scan) || *scan == '_') {
        if (n == NAME_LEN - 1) { fail("Identifier too long (max 23)"); return 0; }
        name[n++] = (char)toupper((unsigned char)*scan++);
    }
    name[n] = 0; return 1;
}
static int endline(void)
{
    space();
    if (!*scan || *scan == '\'') return 1;
    fail("Unexpected text at end of statement"); return 0;
}
static void emit(char *text)
{
    if (*sink_used == OUT_LINES) {
        memmove(sink[0], sink[1], (OUT_LINES - 1) * OUT_LEN); --*sink_used;
    }
    strncpy(sink[*sink_used], text, OUT_LEN - 1);
    sink[*sink_used][OUT_LEN - 1] = 0; ++*sink_used;
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
    if (ncode == PROGRAM_LINES) { problem = "Project exceeds 512 source lines"; return 0; }
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
            fail("Cached source line exceeds 95 characters"); return 0;
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
                problem = "Source line exceeds 95 characters"; failure_file = file; failure_row = row + 1; break;
            }
        }
        if (++row > MAX_LINES) { problem = "File exceeds 64 source lines"; failure_file = file; failure_row = row; break; }
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
    int i;
    for (i = 0; i < nclasses; ++i) if (!strcmp(name, classes[i].name)) return i;
    return -1;
}
static int method_index(int owner, char *name)
{
    int i;
    for (i = 0; i < nmethods; ++i) if (methods[i].owner == owner && !strcmp(name, methods[i].name)) return i;
    return -1;
}
static int field_index(int owner, char *name)
{
    int i;
    for (i = 0; i < classes[owner].fields; ++i) if (!strcmp(name, classes[owner].field[i].name)) return i;
    return -1;
}
static int integer_type(void)
{
    if (word("INTEGER") || word("LONG")) return 1;
    fail("Only Integer and Long numeric types supported"); return 0;
}
static void definitions(void)
{
    int owner, class_start, visibility, function, m, i, end, firstline;
    long initial;
    char name[NAME_LEN], *before;
    Method *method;
    Field *field;
    owner = -1; class_start = -1;
    for (pc = 0; pc < ncode && !problem; ++pc) {
        scan = code[pc].text;
        if (word("REM")) continue;
        space(); if (!*scan || *scan == '\'') continue;
        if (word("IMPORTS")) { if (owner >= 0) fail("Imports must be outside a Class"); continue; }
        visibility = 1;
        if (word("PUBLIC")) visibility = 1;
        else if (word("PRIVATE")) visibility = 0;
        if (word("CLASS")) {
            if (owner >= 0) { fail("Nested Class is not supported"); break; }
            if (!ident(name) || !endline()) break;
            if (class_index(name) >= 0) { fail("Duplicate Class name"); break; }
            if (nclasses == CLASSES) { fail("Maximum 8 Classes"); break; }
            owner = nclasses++; strcpy(classes[owner].name, name); classes[owner].fields = 0;
            class_start = pc; continue;
        }
        before = scan; function = word("FUNCTION");
        if (function || word("SUB")) {
            if (!ident(name)) break;
            if (method_index(owner, name) >= 0) { fail("Duplicate method or procedure"); break; }
            if (owner >= 0 && field_index(owner, name) >= 0) { fail("Field and method have same name"); break; }
            if (nmethods == METHODS) { fail("Maximum 32 methods/procedures"); break; }
            m = nmethods++; method = &methods[m];
            strcpy(method->name, name); method->owner = owner; method->function = function;
            method->visible = visibility; method->argc = 0; method->start = pc + 1;
            space(); if (*scan++ != '(') { fail("Expected parameter list (...)"); break; }
            space();
            while (*scan != ')' && !problem) {
                i = method->argc;
                if (i == PARAMS) { fail("Maximum 4 parameters"); break; }
                if (!ident(method->params[i])) break;
                for (end = 0; end < i; ++end) if (!strcmp(method->params[end], method->params[i])) fail("Duplicate parameter");
                if (word("AS")) integer_type();
                ++method->argc; space();
                if (*scan != ',') break;
                ++scan;
            }
            if (problem) break;
            space(); if (*scan++ != ')') { fail("Expected closing parameter parenthesis"); break; }
            if (function && word("AS")) integer_type();
            if (!endline()) break;
            firstline = pc; end = -1;
            for (++pc; pc < ncode && !problem; ++pc) {
                if (code[pc].file != code[firstline].file) { fail("Method cannot cross file boundaries"); break; }
                scan = code[pc].text;
                if (word("END") && word(function ? "FUNCTION" : "SUB")) { if (endline()) end = pc; break; }
                scan = code[pc].text;
                if (word("PUBLIC") || word("PRIVATE")) space();
                if (word("SUB") || word("FUNCTION") || word("CLASS") || word("IMPORTS")) {
                    fail("Nested definitions / Imports in method not supported"); break;
                }
            }
            if (end < 0 && !problem) { pc = firstline; fail("Missing End Sub / End Function"); }
            if (problem) break;
            method->end = end; skip_to[firstline] = end + 1; pc = end; continue;
        }
        scan = before;
        if (word("END") && word("CLASS")) {
            if (owner < 0) { fail("End Class without Class"); break; }
            if (!endline()) break;
            if (code[pc].file != code[class_start].file) { fail("Class cannot cross file boundaries"); break; }
            skip_to[class_start] = pc + 1; owner = -1; class_start = -1; continue;
        }
        if (owner >= 0) {
            scan = before; word("DIM");
            if (!ident(name) || !word("AS") || !integer_type()) { if (!problem) fail("Expected field As Integer"); break; }
            if (field_index(owner, name) >= 0 || method_index(owner, name) >= 0) { fail("Duplicate member name"); break; }
            i = classes[owner].fields;
            if (i == FIELDS) { fail("Maximum 8 fields per Class"); break; }
            initial = 0; space();
            if (*scan == '=') {
                int negative;
                ++scan; space(); negative = 0;
                if (*scan == '-' || *scan == '+') negative = *scan++ == '-';
                if (!isdigit((unsigned char)*scan)) { fail("Field default must be an integer literal"); break; }
                while (isdigit((unsigned char)*scan)) {
                    initial = initial * 10 + (*scan++ - '0');
                    if (initial > LIMIT) { fail("Number exceeds 1000000"); break; }
                }
                if (negative) initial = -initial;
            }
            if (!endline()) break;
            field = &classes[owner].field[classes[owner].fields++];
            strcpy(field->name, name); field->visible = visibility; field->initial = initial;
        }
    }
    if (!problem && owner >= 0) { pc = class_start; fail("Missing End Class"); }
}
static Symbol *lookup(char *name, int create)
{
    int i, pass, frame;
    Symbol *s;
    for (pass = 0; pass < 2; ++pass) {
        frame = pass ? 0 : level;
        for (i = 0; i < frames[frame].count; ++i)
            if (!strcmp(frames[frame].vars[i].name, name)) return &frames[frame].vars[i];
        if (!level) break;
    }
    if (!create) { fail("Unknown variable"); return 0; }
    if (frames[level].count == SYMBOLS) { fail("Maximum 48 variables per scope"); return 0; }
    s = &frames[level].vars[frames[level].count++];
    strcpy(s->name, name); s->type = -1; s->value = number(0); return s;
}
static Symbol *declare(char *name)
{
    int i;
    for (i = 0; i < frames[level].count; ++i) if (!strcmp(name, frames[level].vars[i].name)) {
        fail("Duplicate variable in this scope"); return 0;
    }
    if (frames[level].count == SYMBOLS) { fail("Maximum 48 variables per scope"); return 0; }
    strcpy(frames[level].vars[frames[level].count].name, name);
    return &frames[level].vars[frames[level].count++];
}
static long numeric(Value value)
{
    if (value.kind != NUMBER) { fail("Expected numeric expression"); return 0; }
    return value.number;
}
static int arguments(Value *args)
{
    int count;
    space(); count = 0;
    if (*scan++ != '(') { fail("Expected ("); return 0; }
    space();
    while (*scan != ')' && !problem) {
        if (count == PARAMS) { fail("Maximum 4 arguments"); return count; }
        args[count++] = expression(); space();
        if (*scan != ',') break;
        ++scan;
    }
    space(); if (*scan == ')') ++scan; else fail("Expected closing argument parenthesis");
    return count;
}
static int object_type(Value value)
{
    if (value.kind != REFERENCE) { fail("Expected object reference"); return -1; }
    if (value.object < 0) { fail("Object is Nothing; use New first"); return -1; }
    return objects[value.object].type;
}
static Value self_value(void)
{
    Value value;
    value = number(0); value.kind = REFERENCE; value.object = frames[level].self;
    if (value.object < 0) fail("Me is only valid inside a Class method");
    return value;
}
static int permitted(int owner, int visible)
{
    if (visible || (level && frames[level].self >= 0 && objects[frames[level].self].type == owner)) return 1;
    fail("Private member is not accessible here"); return 0;
}
static Value read_name(char *name, int need_value)
{
    Symbol *s;
    Value v, args[PARAMS];
    char member[NAME_LEN];
    int m, argc, type, f;
    v = number(0); space();
    if (!strcmp(name, "ME")) v = self_value();
    else if (!strcmp(name, "NOTHING")) { v.kind = REFERENCE; v.object = -1; }
    else if (*scan == '(') {
        m = method_index(-1, name); argc = arguments(args);
        if (m < 0) fail("Unknown procedure or Function");
        else if (!problem) v = invoke(m, -1, args, argc, need_value);
    } else {
        s = lookup(name, strlen(name) == 1 && name[0] >= 'A' && name[0] <= 'Z');
        if (s) v = s->value;
    }
    space();
    while (*scan == '.' && !problem) {
        ++scan; if (!ident(member)) break;
        type = object_type(v); if (problem) break;
        space();
        if (*scan == '(') {
            m = method_index(type, member); argc = arguments(args);
            if (m < 0) fail("Unknown object method");
            else if (!problem && permitted(type, methods[m].visible)) v = invoke(m, v.object, args, argc, need_value);
        } else {
            f = field_index(type, member);
            if (f < 0) fail("Unknown object field");
            else if (permitted(type, classes[type].field[f].visible)) v = objects[v.object].field[f];
        }
        space();
    }
    return v;
}
static Value factor(void)
{
    Value v, args[PARAMS];
    long n;
    int sign, type, obj, i, argc, m;
    char name[NAME_LEN];
    v = number(0); space();
    if (++expr_depth > 16) { fail("Expression nested too deeply"); --expr_depth; return v; }
    sign = 1;
    while (*scan == '+' || *scan == '-') { if (*scan++ == '-') sign = -sign; space(); }
    if (*scan == '(') {
        ++scan; v = expression(); space();
        if (*scan == ')') ++scan; else fail("Missing closing parenthesis");
    } else if (isdigit((unsigned char)*scan)) {
        n = 0;
        while (isdigit((unsigned char)*scan)) {
            n = n * 10 + (*scan++ - '0');
            if (n > LIMIT) { fail("Number exceeds 1000000"); n = 0; break; }
        }
        v = number(n);
    } else if (word("NEW")) {
        if (!ident(name)) { --expr_depth; return v; }
        type = class_index(name); argc = 0; space();
        if (*scan == '(') argc = arguments(args);
        if (type < 0) fail("Unknown Class after New");
        else if (nobjects == OBJECTS) fail("Maximum 32 object instances per run");
        if (!problem) {
            obj = nobjects++; objects[obj].type = type;
            for (i = 0; i < classes[type].fields; ++i) objects[obj].field[i] = number(classes[type].field[i].initial);
            m = method_index(type, "NEW");
            if (m >= 0 && permitted(type, methods[m].visible)) invoke(m, obj, args, argc, 0);
            else if (argc) fail("Class has no constructor accepting arguments");
            v.kind = REFERENCE; v.object = obj;
        }
    } else if (ident(name)) v = read_name(name, 1);
    if (sign == -1) v = number(-numeric(v));
    --expr_depth; return v;
}
static Value product(void)
{
    Value a, b;
    long x, y;
    char op;
    a = factor(); space();
    while (!problem && (*scan == '*' || *scan == '/')) {
        op = *scan++; b = factor(); x = numeric(a); y = numeric(b);
        if (problem) break;
        if (op == '*') {
            if (y && (x < 0 ? -x : x) > LIMIT / (y < 0 ? -y : y)) fail("Result exceeds numeric range");
            else x *= y;
        } else if (!y) fail("Division by zero"); else x /= y;
        a = number(x); space();
    }
    return a;
}
static Value sum(void)
{
    Value a, b;
    long x, y;
    char op;
    a = product(); space();
    while (!problem && (*scan == '+' || *scan == '-')) {
        op = *scan++; b = product(); x = numeric(a); y = numeric(b);
        if (problem) break;
        x = op == '+' ? x + y : x - y;
        if (x > LIMIT || x < -LIMIT) fail("Result exceeds numeric range");
        a = number(x); space();
    }
    return a;
}
static Value expression(void)
{
    Value a, b;
    long x, y;
    int result;
    char op, next;
    a = sum(); space();
    if (!problem && (*scan == '=' || *scan == '<' || *scan == '>')) {
        op = *scan++; next = 0;
        if (*scan == '=' || (op == '<' && *scan == '>')) next = *scan++;
        b = sum(); x = numeric(a); y = numeric(b); result = 0;
        if (op == '=') result = x == y;
        else if (op == '<') result = next == '=' ? x <= y : next == '>' ? x != y : x < y;
        else result = next == '=' ? x >= y : x > y;
        a = number(result ? -1 : 0);
    }
    return a;
}
static int assign(Symbol *s, Value *target, Value value)
{
    int type;
    if (problem) return 0;
    if (s && s->type >= 0) {
        if (value.kind != REFERENCE) { fail("Object variable requires object or Nothing"); return 0; }
        if (value.object >= 0 && objects[value.object].type != s->type) { fail("Object Class does not match declaration"); return 0; }
    } else {
        type = value.kind;
        if (type != NUMBER) { fail("Numeric variable/field requires number"); return 0; }
    }
    *target = value; return 1;
}
static void print_line(void)
{
    char text[OUT_LEN], item[OUT_LEN];
    int len, n;
    Value v;
    text[0] = 0; len = 0;
    while (!problem) {
        space(); n = 0;
        if (*scan == '"') {
            ++scan;
            while (*scan && *scan != '"') {
                if (n == OUT_LEN - 1) { fail("PRINT output exceeds 79 characters"); break; }
                item[n++] = *scan++;
            }
            item[n] = 0;
            if (*scan == '"') ++scan; else fail("Missing closing quote");
        } else if (!*scan || *scan == '\'') item[0] = 0;
        else { v = expression(); sprintf(item, "%ld", numeric(v)); n = strlen(item); }
        if (problem) break;
        if (len + n >= OUT_LEN) { fail("PRINT output exceeds 79 characters"); break; }
        strcat(text, item); len += n; space();
        if (*scan != ';') break;
        ++scan;
    }
    if (endline() && !problem) emit(text);
}
static void statement(void)
{
    char name[NAME_LEN], member[NAME_LEN], type_name[NAME_LEN];
    Symbol *s;
    Value v, object, *target, args[PARAMS];
    int type, field, method, argc;
    char *initializer;
    scan = code[pc].text; expr_depth = 0;
    space(); if (!*scan || *scan == '\'' || word("REM") || word("IMPORTS")) return;
    if (word("PRINT")) { print_line(); return; }
    if (word("RETURN")) {
        if (!level) { fail("Return is only valid inside a method"); return; }
        if (methods[frames[level].method].function) { return_value = expression(); numeric(return_value); }
        else return_value = number(0);
        if (endline()) returned = 1;
        return;
    }
    if (word("EXIT")) {
        if (!level || !word(methods[frames[level].method].function ? "FUNCTION" : "SUB")) { fail("Expected Exit Sub / Exit Function inside method"); return; }
        if (methods[frames[level].method].function) { fail("Function must Return a value"); return; }
        if (endline()) returned = 1;
        return;
    }
    if (word("END")) {
        if (level) fail("Use Return or Exit Sub inside a method");
        else if (endline()) stopped = 1;
        return;
    }
    if (word("DIM")) {
        if (!ident(name) || !word("AS")) { if (!problem) fail("Expected Dim name As type"); return; }
        s = declare(name); if (!s) return;
        s->type = -1; s->value = number(0);
        initializer = scan;
        if (word("NEW")) {
            if (!ident(type_name)) return;
            type = class_index(type_name); if (type < 0) { fail("Unknown Class in Dim"); return; }
            /* factor owns allocation and constructor semantics. */
            scan = initializer;
            s->type = type; v = expression(); assign(s, &s->value, v);
        } else {
            if (!ident(type_name)) return;
            if (!strcmp(type_name, "INTEGER") || !strcmp(type_name, "LONG")) s->type = -1;
            else {
                type = class_index(type_name); if (type < 0) { fail("Unknown variable type"); return; }
                s->type = type; s->value.kind = REFERENCE; s->value.object = -1;
            }
            space(); if (*scan == '=') { ++scan; v = expression(); assign(s, &s->value, v); }
        }
        endline(); return;
    }
    if (word("CALL")) {
        if (ident(name)) read_name(name, 0);
        endline(); return;
    }
    word("LET");
    if (!ident(name)) return;
    space(); s = 0; target = 0;
    if (*scan == '(') {
        method = method_index(-1, name); argc = arguments(args);
        if (method < 0) fail("Unknown procedure");
        else if (!problem) invoke(method, -1, args, argc, 0);
        endline(); return;
    }
    if (*scan == '.') {
        if (!strcmp(name, "ME")) object = self_value();
        else { s = lookup(name, 0); object = s ? s->value : number(0); }
        type = object_type(object); if (problem) return;
        ++scan; if (!ident(member)) return;
        space(); s = 0;
        if (*scan == '(') {
            method = method_index(type, member); argc = arguments(args);
            if (method < 0) fail("Unknown object method");
            else if (!problem && permitted(type, methods[method].visible)) invoke(method, object.object, args, argc, 0);
            endline(); return;
        }
        field = field_index(type, member);
        if (field < 0) { fail("Unknown object field"); return; }
        if (!permitted(type, classes[type].field[field].visible)) return;
        target = &objects[object.object].field[field];
    } else {
        s = lookup(name, 1); if (!s) return;
        target = &s->value;
    }
    space(); if (*scan++ != '=') { fail("Expected assignment or method call"); return; }
    v = expression(); if (endline()) assign(s, target, v);
}
static Value invoke(int m, int self, Value *args, int argc, int need_value)
{
    char *saved_scan;
    int saved_pc, saved_depth, saved_returned, i;
    Value saved_return, result;
    Method *method;
    Frame *frame;
    method = &methods[m]; result = number(0);
    if (argc != method->argc) { fail("Wrong number of method arguments"); return result; }
    if (need_value && !method->function) { fail("Sub cannot be used as an expression"); return result; }
    if (level == CALL_DEPTH) { fail("Maximum method call depth is 8"); return result; }
    for (i = 0; i < argc; ++i) numeric(args[i]);
    if (problem) return result;
    saved_scan = scan; saved_pc = pc; saved_depth = expr_depth;
    saved_returned = returned; saved_return = return_value;
    frame = &frames[++level]; frame->self = self; frame->method = m; frame->count = argc;
    for (i = 0; i < argc; ++i) {
        strcpy(frame->vars[i].name, method->params[i]); frame->vars[i].type = -1; frame->vars[i].value = args[i];
    }
    returned = 0; return_value = number(0);
    for (pc = method->start; pc < method->end && !problem && !returned; ++pc) statement();
    if (!problem && method->function && !returned) { pc = method->end; fail("Function finished without Return"); }
    if (!problem) result = return_value;
    --level; scan = saved_scan; pc = saved_pc; expr_depth = saved_depth;
    returned = saved_returned; return_value = saved_return;
    return result;
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
int basic_run_project(char source[MAX_LINES][LINE_LEN], int count,
                      char *mainpath, char output[OUT_LINES][OUT_LEN], int *used)
{
    char path[BASIC_PATH], text[OUT_LEN];
    int i;
    sink = output; sink_used = used; *used = 0;
    ncode = nfiles = nclasses = nmethods = nobjects = level = returned = stopped = 0;
    problem = 0; pc = -1; failure_file = 0; failure_row = 1;
    memset(frames, 0, sizeof(frames)); memset(skip_to, 0, sizeof(skip_to));
    frames[0].self = frames[0].method = -1;
    if (count < 0 || count > MAX_LINES) problem = "Invalid main source line count";
    for (i = 0; i < count && !problem; ++i) if (!memchr(source[i], 0, LINE_LEN)) problem = "Source line exceeds 95 characters";
    if (!problem && !path_join(path, "", mainpath)) problem = "Invalid main source path";
    if (!problem) load_module(path, source, count, 0);
    if (!problem) definitions();
    for (pc = 0; pc < ncode && !problem && !stopped; ) {
        if (skip_to[pc]) pc = skip_to[pc];
        else { statement(); ++pc; }
    }
    if (problem) {
        sprintf(text, "%.24s:%d: %.44s", basic_file_name(failure_file), failure_row, problem);
        emit(text); return failure_row;
    }
    emit("Program finished."); return 0;
}
int basic_run(char source[MAX_LINES][LINE_LEN], int count, char output[OUT_LINES][OUT_LEN], int *used)
{
    return basic_run_project(source, count, "MAIN.BAS", output, used);
}
