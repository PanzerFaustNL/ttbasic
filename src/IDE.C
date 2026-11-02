/* TT BASIC IDE 0.4 - Pure C / GEM / TT Medium. */
#include <aes.h>
#include <vdi.h>
#include <stdio.h>
#include <string.h>
#include "BASIC.H"
#include "MENU.H"
#include "LANG.H"
#include <ctype.h>
#define SOURCE "C:\\TTBASIC\\MAIN.BAS"
#define TEMP "C:\\TTBASIC\\MAIN.TMP"
#define BACKUP "C:\\TTBASIC\\MAIN.BAK"
#define KIND (NAME | CLOSER | MOVER)
static int window, vdi, wx, wy, ww, wh, cw, ch;
static int row, col, first, leftcol, lines = 1, changed, props;
static int editor_x, editor_y, editor_w, editor_h, cols, rows;
static int out_used, error_line, done;
static int show_tree = 1, show_output = 1, tree_expanded = 1;
static int undo_ready, undo_row, undo_col, undo_lines, undo_changed;
static char undo_source[MAX_LINES][LINE_LEN];
static char clipboard[LINE_LEN], query[25];
static int clipboard_ready;
static void action(int command);
static char source[MAX_LINES][LINE_LEN];
static char output[OUT_LINES][OUT_LEN];
static char status[80] = "Gereed | F1 Help  F5 Uitvoeren";
static char title[100] = "TT BASIC Studio 0.4 - MAIN.BAS";
typedef struct {
    char path[BASIC_PATH], text[MAX_LINES][LINE_LEN];
    int count, dirty, row, col, first, left;
} Document;
static Document documents[BASIC_FILES];
static int document_count = 1, active_document;
static char *document_name(void)
{
    char *p, *name;
    name = documents[active_document].path;
    for (p = name; *p; ++p) if (*p == '/' || *p == '\\') name = p + 1;
    return name;
}
static void cache_current(void)
{
    Document *d;
    d = &documents[active_document]; memcpy(d->text, source, sizeof(source));
    d->count = lines; d->dirty = changed; d->row = row; d->col = col;
    d->first = first; d->left = leftcol;
}
static int path_equal(char *a, char *b)
{
    while (*a && *b) {
        if ((*a == '/' || *a == '\\') && (*b == '/' || *b == '\\')) { ++a; ++b; }
        else if (toupper((unsigned char)*a++) != toupper((unsigned char)*b++)) return 0;
    }
    return !*a && !*b;
}
static BasicSource cached_file(char *path, int *count)
{
    int i;
    for (i = 0; i < document_count; ++i) if (path_equal(path, documents[i].path)) {
        *count = documents[i].count; return documents[i].text;
    }
    return 0;
}
static void scan_files(void)
{
    cache_current();
    basic_scan_project(documents[0].text, documents[0].count, SOURCE);
}
static void activate_document(int index)
{
    Document *d;
    active_document = index; d = &documents[index];
    memcpy(source, d->text, sizeof(source)); lines = d->count;
    changed = d->dirty; row = d->row; col = d->col;
    first = d->first; leftcol = d->left; undo_ready = error_line = 0;
    sprintf(title, "TT BASIC Studio 0.4 - %.64s", document_name());
    wind_set(window, WF_NAME, title);
}
static void open_import(int index);
static int discard_all(void);
static int minimum(int a, int b) { return a < b ? a : b; }
static int maximum(int a, int b) { return a > b ? a : b; }
static void fill(int x, int y, int w, int h, int color)
{
    int p[4];
    if (w <= 0 || h <= 0) return;
    p[0] = x; p[1] = y; p[2] = x + w - 1; p[3] = y + h - 1;
    vsf_color(vdi, color); v_bar(vdi, p);
}
static void text(int x, int y, int color, char *s, int width)
{
    char buf[100];
    int n;
    n = minimum((int)strlen(s), minimum(width / cw, 99));
    if (n <= 0) return;
    memcpy(buf, s, n); buf[n] = 0;
    vst_color(vdi, color); vswr_mode(vdi, 2);
    v_gtext(vdi, x, y, buf); vswr_mode(vdi, 1);
}
static void button(int x, int width, char *label, int active)
{
    fill(x, wy + 5, width, 24, active ? 4 : 0);
    text(x + 8, wy + 22, active ? 0 : 1, label, width - 16);
}
static void layout(void)
{
    wind_get(window, WF_WORKXYWH, &wx, &wy, &ww, &wh);
    editor_x = wx + (show_tree ? 130 : 6); editor_y = wy + 64;
    editor_w = ww - (editor_x - wx) - 6 - (props ? 138 : 0);
    editor_h = wh - 64 - (show_output ? 118 : 28);
    cols = maximum(1, (editor_w - 40) / cw);
    rows = maximum(1, editor_h / ch);
    if (row < first) first = row;
    if (row >= first + rows) first = row - rows + 1;
    if (col < leftcol) leftcol = col;
    if (col >= leftcol + cols) leftcol = col - cols + 1;
}
static int sameword(char *a, char *b, int len)
{
    int i;
    if ((int)strlen(b) != len) return 0;
    for (i = 0; i < len; ++i)
        if (toupper((unsigned char)a[i]) != b[i]) return 0;
    return 1;
}
static void code_line(int number, int y)
{
    char *s, buf[LINE_LEN];
    int start, end, i, color, visible, x;
    s = source[number]; start = 0;
    while (s[start]) {
        end = start + 1; color = 1;
        if (s[start] == '\'') { end = strlen(s); color = 9; }
        else if (s[start] == '"') {
            while (s[end] && s[end] != '"') ++end;
            if (s[end]) ++end;
            color = 4;
        } else if (isalpha((unsigned char)s[start])) {
            while (isalnum((unsigned char)s[end]) || s[end] == '_') ++end;
            if (sameword(s + start, "REM", end - start)) { end = strlen(s); color = 9; }
            else {
                char keyword[LINE_LEN];
                memcpy(keyword,s+start,end-start); keyword[end-start]=0;
                if (basic_is_reserved(keyword)) color=4;
            }
        }
        visible = maximum(start, leftcol);
        i = minimum(end, leftcol + cols) - visible;
        if (i > 0) {
            memcpy(buf, s + visible, i); buf[i] = 0;
            x = editor_x + 36 + (visible - leftcol) * cw;
            text(x, y + ch - 3, color, buf, i * cw);
        }
        start = end;
    }
}
static void heading(int x, int y, int width, char *label)
{
    fill(x, y, width, 24, 8);
    fill(x, y + 23, width, 1, 9);
    text(x + 7, y + 18, 1, label, width - 14);
}
static void paint(void)
{
    int i, y, line, bottom, panel, imported;
    char buf[100];
    layout(); bottom = wy + wh - 118;
    fill(wx, wy, ww, wh, 8);
    button(wx + 6, 94, tr(T_LOAD_BUTTON), 0);
    button(wx + 106, 112, tr(T_SAVE_BUTTON), 0);
    button(wx + 224, 136, tr(T_RUN_BUTTON), 1);
    text(wx + 376, wy + 22, 1, "TT BASIC Studio", ww - 388);
    fill(wx, wy + 35, ww, 1, 9);
    if (show_tree) {
        fill(wx + 6, wy + 40, 118, wh - 68, 0);
        heading(wx + 6, wy + 40, 118, tr(T_PROJECT));
        text(wx + 12, wy + 84, 1, tree_expanded ? "- Demo" : "+ Demo", 106);
        if (tree_expanded) {
            fill(wx + 20, wy + 94, 1, 40, 9);
            text(wx + 28, wy + 110, 9, tr(T_SOURCE), 90);
            fill(wx + 24, wy + 119, 98, 22, active_document == 0 ? 4 : 8);
            text(wx + 27, wy + 136, active_document == 0 ? 0 : 1, "MAIN.BAS", 92);
        }
        if (tree_expanded && basic_file_count() > 1) {
            text(wx + 13, wy + 171, 9, "Imports", 106);
            imported = basic_file_count() - 1;
            for (i = 1; i <= imported; ++i) {
                y = wy + 175 + i * 18;
                if (y >= wy + wh - 60) break;
                text(wx + 20, y, path_equal(basic_file_path(i), documents[active_document].path) ? 4 : 1, basic_file_name(i), 100);
            }
            if (i <= imported) text(wx + 20, y, 9, "...", 100);
        }
        sprintf(buf, tr(T_FILE_COUNT), maximum(1, basic_file_count()));
        text(wx + 13, wy + wh - 44, 9, buf, 106);
    }
    fill(editor_x, wy + 40, editor_w, 24, 0);
    fill(editor_x, wy + 40, minimum(144, editor_w), 3, 4);
    sprintf(buf, "%.60s%s", document_name(), changed ? " *" : "");
    text(editor_x + 8, wy + 58, 1, buf, editor_w - 16);
    fill(editor_x, editor_y, editor_w, editor_h, 0);
    fill(editor_x, editor_y, 32, editor_h, 8);
    for (i = 0; i < rows && first + i < lines; ++i) {
        line = first + i; y = editor_y + i * ch;
        if (line == row) {
            fill(editor_x + 33, y, editor_w - 33, ch, 8);
            fill(editor_x + 32, y, 2, ch, 4);
        }
        sprintf(buf, "%2d", line + 1);
        text(editor_x + 4, y + ch - 3, error_line == line + 1 ? 2 : 9, buf, 28);
        code_line(line, y);
        if (line == row) fill(editor_x + 36 + (col - leftcol) * cw, y + ch - 2, cw, 2, 4);
    }
    if (props) {
        panel = editor_x + editor_w + 6;
        fill(panel, wy + 40, 132, editor_h + 24, 0);
        heading(panel, wy + 40, 132, tr(T_PROPS));
        text(panel + 7, wy + 87, 9, tr(T_FILENAME), 118);
        text(panel + 7, wy + 107, 1, document_name(), 118);
        text(panel + 7, wy + 139, 9, tr(T_LANGUAGE), 118);
        text(panel + 7, wy + 159, 1, "TT BASIC", 118);
        text(panel + 7, wy + 191, 9, tr(T_LINES), 118);
        sprintf(buf, "%d / 256", lines); text(panel + 7, wy + 211, 1, buf, 118);
        text(panel + 7, wy + 243, 9, tr(T_STATUS), 118);
        text(panel + 7, wy + 263, 1, changed ? tr(T_MODIFIED) : tr(T_SAVED), 118);
    }
    if (show_output) {
        heading(editor_x, bottom, ww - (editor_x - wx) - 6, tr(T_OUTPUT));
        fill(editor_x, bottom + 24, ww - (editor_x - wx) - 6, 68, 0);
        i = maximum(0, out_used - 4);
        for (y = bottom + 24 + ch - 3; i < out_used && y < bottom + 92; ++i, y += ch)
            text(editor_x + 7, y, error_line ? 2 : 1, output[i], ww - (editor_x - wx) - 20);
        if (!out_used) text(editor_x + 7, bottom + 42, 9, tr(T_RUN_HINT), ww - (editor_x - wx) - 20);
    }
    fill(wx, wy + wh - 22, ww, 22, 4);
    sprintf(buf, "Ln %d : %d | %s", row + 1, col + 1, status);
    text(wx + 8, wy + wh - 6, 0, buf, ww - 16);
}
static void redraw(int x, int y, int w, int h)
{
    int rx, ry, rw, rh, x1, y1, x2, y2, clip[4];
    wind_update(BEG_UPDATE); graf_mouse(M_OFF, 0L);
    wind_get(window, WF_FIRSTXYWH, &rx, &ry, &rw, &rh);
    while (rw > 0 && rh > 0) {
        x1 = maximum(x, rx); y1 = maximum(y, ry);
        x2 = minimum(x + w, rx + rw); y2 = minimum(y + h, ry + rh);
        if (x1 < x2 && y1 < y2) {
            clip[0] = x1; clip[1] = y1; clip[2] = x2 - 1; clip[3] = y2 - 1;
            vs_clip(vdi, 1, clip); paint();
        }
        wind_get(window, WF_NEXTXYWH, &rx, &ry, &rw, &rh);
    }
    vs_clip(vdi, 0, clip); graf_mouse(M_ON, 0L); wind_update(END_UPDATE);
}
static void refresh(void)
{
    menus_state(undo_ready, clipboard_ready, show_tree, props, show_output);
    layout(); redraw(wx, wy, ww, wh);
}
static void remember(void)
{
    memcpy(undo_source, source, sizeof(source));
    undo_row = row; undo_col = col; undo_lines = lines;
    undo_changed = changed; undo_ready = 1;
}
static void undo_edit(void)
{
    if (!undo_ready) return;
    memcpy(source, undo_source, sizeof(source));
    row = undo_row; col = undo_col; lines = undo_lines;
    changed = undo_changed; undo_ready = 0; error_line = 0;
    scan_files();
    strcpy(status, tr(T_UNDO_STATUS));
}
static int discard(void)
{
    return !changed || form_alert(1, tr(T_DISCARD)) == 2;
}
static void open_source(void)
{
    FILE *f;
    static char incoming[MAX_LINES][LINE_LEN];
    char buf[LINE_LEN + 3];
    int n, len, failed;
    if (!discard()) return;
    f = fopen(documents[active_document].path, "r");
    if (!f) { strcpy(status, tr(T_OPEN_FAILED)); return; }
    memset(incoming, 0, sizeof(incoming)); n = 0; failed = 0;
    while (fgets(buf, sizeof(buf), f)) {
        len = strlen(buf);
        while (len && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) buf[--len] = 0;
        if (n == MAX_LINES || len >= LINE_LEN) { failed = 1; break; }
        strcpy(incoming[n++], buf);
    }
    if (ferror(f)) failed = 1;
    fclose(f);
    if (failed) { strcpy(status, tr(T_FILE_TOO_BIG)); return; }
    memcpy(source, incoming, sizeof(source)); lines = n ? n : 1;
    row = col = first = leftcol = changed = error_line = undo_ready = 0;
    scan_files();
    sprintf(status, tr(T_OPENED), document_name());
}
static void save_source(void)
{
    FILE *f;
    int i, failed, len;
    char tmp[BASIC_PATH], backup[BASIC_PATH], *extension;
    strcpy(tmp, documents[active_document].path);
    extension = strrchr(tmp, '.');
    if (!extension || !path_equal(extension, ".BAS")) {
        strcpy(status, tr(T_SAVE_EXT)); return;
    }
    len = (int)(extension - tmp);
    strcpy(tmp + len, ".TMP"); strcpy(backup, tmp); strcpy(backup + len, ".BAK");
    f = fopen(tmp, "w");
    if (!f) { strcpy(status, tr(T_TEMP_FAILED)); return; }
    failed = 0;
    for (i = 0; i < lines; ++i) if (fprintf(f, "%s\n", source[i]) < 0) failed = 1;
    if (fclose(f)) failed = 1;
    if (failed) { strcpy(status, tr(T_WRITE_FAILED)); return; }
    f = fopen(documents[active_document].path, "r");
    if (f) {
        fclose(f); remove(backup);
        if (rename(documents[active_document].path, backup)) { strcpy(status, tr(T_BACKUP_FAILED)); return; }
    }
    if (rename(tmp, documents[active_document].path)) {
        rename(backup, documents[active_document].path); strcpy(status, tr(T_SAVE_FAILED)); return;
    }
    changed = 0; undo_ready = 0; scan_files();
    sprintf(status, tr(T_SAVE_OK), document_name());
}
static int runtime_input(char *prompt,char *buffer,int capacity)
{
    strcpy(status,tr(T_INPUT_WAIT)); refresh();
    return input_dialog(prompt,buffer,capacity);
}
static int runtime_poll(void)
{
    int message[8],events,mx,my,mb,ks,key,clicks;
    events=evnt_multi(MU_KEYBD|MU_TIMER|MU_MESAG,0,0,0,
        0,0,0,0,0,0,0,0,0,0,message,1,0,&mx,&my,&mb,&ks,&key,&clicks);
    if ((events & MU_KEYBD) && (key & 255)==27) return 0;
    if (events & MU_MESAG) {
        if (message[0]==MN_SELECTED) {
            if (menus_event(message[3],message[4])==ACT_QUIT) return 0;
        } else if (message[3]==window) {
            if (message[0]==WM_CLOSED) return 0;
            if (message[0]==WM_REDRAW) redraw(message[4],message[5],message[6],message[7]);
            else if (message[0]==WM_TOPPED) wind_set(window,WF_TOP,0,0,0,0);
            else if (message[0]==WM_MOVED) wind_set(window,WF_CURRXYWH,message[4],message[5],message[6],message[7]);
        }
    }
    return 1;
}
static void run_source(void)
{
    show_output = 1;
    cache_current(); strcpy(status,tr(T_RUNNING)); refresh();
    error_line = basic_run_project(documents[0].text, documents[0].count, SOURCE, output, &out_used);
    if (error_line) {
        if (!path_equal(documents[active_document].path, basic_error_path()))
            error_line = active_document == 0 ? basic_error_main_line() : -1;
    }
    if (error_line) {
        if (error_line > 0) { row = minimum(lines - 1, error_line - 1); col = 0; }
        strcpy(status, tr(T_BASIC_ERROR));
    }
    else strcpy(status, tr(T_FINISHED));
}
static void find_next(int beginning)
{
    int r, c, step, start;
    if (!query[0]) {
        if (!search_dialog(query)) return;
        beginning = 1;
    }
    for (step = 0; step <= lines; ++step) {
        r = (row + step) % lines;
        start = step == 0 ? col + (beginning ? 0 : 1) : 0;
        if (step == lines) start = 0;
        for (c = start; c + (int)strlen(query) <= (int)strlen(source[r]); ++c) {
            int k, match;
            if (step == lines && c >= col) break;
            match = 1;
            for (k = 0; query[k]; ++k)
                if (toupper((unsigned char)source[r][c + k]) != toupper((unsigned char)query[k])) match = 0;
            if (match) { row = r; col = c; strcpy(status, tr(T_FOUND)); return; }
        }
    }
    strcpy(status, tr(T_NOT_FOUND));
}
static void open_import(int index)
{
    char path[BASIC_PATH], buf[LINE_LEN + 3];
    FILE *f;
    Document *d;
    int i, n, len, failed;
    if (index < 0 || index >= basic_file_count()) return;
    strcpy(path, basic_file_path(index)); cache_current();
    for (i = 0; i < document_count; ++i) if (path_equal(path, documents[i].path)) {
        activate_document(i); return;
    }
    if (document_count == BASIC_FILES) { strcpy(status, tr(T_DOC_LIMIT)); return; }
    f = fopen(path, "r");
    if (!f) { strcpy(status, tr(T_OPEN_FAILED)); return; }
    d = &documents[document_count]; memset(d, 0, sizeof(Document));
    n = failed = 0;
    while (fgets(buf, sizeof(buf), f)) {
        len = strlen(buf);
        while (len && (buf[len - 1] == '\r' || buf[len - 1] == '\n')) buf[--len] = 0;
        if (n == MAX_LINES || len >= LINE_LEN) { failed = 1; break; }
        strcpy(d->text[n++], buf);
    }
    if (ferror(f)) failed = 1;
    fclose(f);
    if (failed) { strcpy(status, tr(T_FILE_TOO_BIG)); return; }
    strcpy(d->path, path); d->count = n ? n : 1;
    activate_document(document_count++);
    sprintf(status, tr(T_MODULE_OPENED), document_name());
}
static int discard_all(void)
{
    int i, count;
    cache_current(); count = 0;
    for (i = 0; i < document_count; ++i) if (documents[i].dirty) ++count;
    if (!count) return 1;
    return form_alert(1, tr(T_CLOSE_DIRTY)) == 2;
}
static void action(int command)
{
    int i;
    switch (command) {
    case ACT_NEW:
        cache_current();
        if (active_document) activate_document(0);
        if (!discard()) break;
        remember(); memset(source, 0, sizeof(source)); lines = 1;
        row = col = first = leftcol = error_line = out_used = 0;
        scan_files();
        changed = 1; strcpy(status, tr(T_NEW_STATUS)); break;
    case ACT_FLOW:
    case ACT_POLY:
    case ACT_INPUT:
    case ACT_DEMO:
        cache_current();
        if (active_document) activate_document(0);
        if (!discard()) break;
        {
            FILE *demo;
            char buf[LINE_LEN];
            int n, len;
            demo = fopen(command==ACT_FLOW ? "C:\\TTBASIC\\FLOW.BAS" :
                command==ACT_POLY ? "C:\\TTBASIC\\POLY.BAS" :
                command==ACT_INPUT ? "C:\\TTBASIC\\INPUT.BAS" : "C:\\TTBASIC\\OODEMO.BAS", "r");
            if (!demo) { strcpy(status, tr(T_DEMO_MISSING)); break; }
            remember(); memset(source, 0, sizeof(source)); n = 0;
            while (n < MAX_LINES && fgets(buf, sizeof(buf), demo)) {
                len = strlen(buf);
                while (len && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) buf[--len] = 0;
                strcpy(source[n++], buf);
            }
            fclose(demo); lines = n ? n : 1;
            row = col = first = leftcol = error_line = out_used = 0; changed = 1;
            scan_files();
            strcpy(status, tr(T_DEMO_STATUS));
        }
        break;
    case ACT_OPEN: open_source(); break;
    case ACT_SAVE: save_source(); break;
    case ACT_QUIT: done = discard_all(); break;
    case ACT_RUN: run_source(); break;
    case ACT_UNDO: undo_edit(); break;
    case ACT_COPY:
    case ACT_CUT:
        strcpy(clipboard, source[row]); clipboard_ready = 1;
        strcpy(status, tr(T_COPY_STATUS));
        if (command == ACT_COPY) break;
        remember();
        for (i = row; i < lines - 1; ++i) strcpy(source[i], source[i + 1]);
        if (lines > 1) --lines; else source[0][0] = 0;
        row = minimum(row, lines - 1); col = 0; changed = 1;
        strcpy(status, tr(T_CUT_STATUS)); break;
    case ACT_PASTE:
        if (!clipboard_ready) break;
        if (lines == MAX_LINES) { strcpy(status, tr(T_LINE_LIMIT)); break; }
        remember();
        if (source[row][0]) {
            for (i = lines; i > row; --i) strcpy(source[i], source[i - 1]);
            ++lines;
        }
        strcpy(source[row], clipboard); col = 0; changed = 1;
        strcpy(status, tr(T_PASTE_STATUS)); break;
    case ACT_FIND: if (search_dialog(query)) find_next(1); break;
    case ACT_NEXT: find_next(0); break;
    case ACT_CLEAR: out_used = error_line = 0; strcpy(status, tr(T_CLEAR_STATUS)); break;
    case ACT_TREE: show_tree = !show_tree; break;
    case ACT_PROPS: props = !props; break;
    case ACT_OUTPUT: show_output = !show_output; break;
    case ACT_LANG_NL:
    case ACT_LANG_EN:
    case ACT_LANG_DE:
        menus_language(command - ACT_LANG_NL);
        {
            FILE *settings;
            settings = fopen("C:\\TTBASIC\\STUDIO.CFG", "w");
            if (settings) { fprintf(settings, "%d\n", language_get()); fclose(settings); }
        }
        strcpy(status, tr(T_LANG_CHANGED)); break;
    case ACT_ABOUT:
        form_alert(1, tr(T_ABOUT)); break;
    case ACT_KEYS:
        form_alert(1, tr(T_KEYS)); break;
    }
}
static void edit(int key, int state)
{
    int ascii, scan, len, i;
    char tail[LINE_LEN];
    ascii = key & 255; scan = (key >> 8) & 255; len = strlen(source[row]);
    if (state & 4) {
        switch (scan) {
        case 31: action(ACT_SAVE); return;
        case 24: action(ACT_OPEN); return;
        case 49: action(ACT_NEW); return;
        case 16: action(ACT_QUIT); return;
        case 44: action(ACT_UNDO); return;
        case 46: action(ACT_COPY); return;
        case 45: action(ACT_CUT); return;
        case 47: action(ACT_PASTE); return;
        case 33: action(ACT_FIND); return;
        }
    }
    if (scan == 59) { action(ACT_KEYS); return; }
    if (scan == 60) { action(ACT_SAVE); return; }
    if (scan == 61) { action(ACT_OPEN); return; }
    if (scan == 62) { action(ACT_PROPS); return; }
    if (scan == 63) { action(ACT_RUN); return; }
    if (scan == 64) { action(ACT_NEXT); return; }
    if (scan == 72) { if (row) --row; }
    else if (scan == 80) { if (row < lines - 1) ++row; }
    else if (scan == 75) { if (col) --col; else if (row) { --row; col = strlen(source[row]); } }
    else if (scan == 77) { if (col < len) ++col; else if (row < lines - 1) { ++row; col = 0; } }
    else if (scan == 71) col = 0;
    else if (ascii == 8 || scan == 83) {
        if (ascii == 8 && col) {
            remember(); memmove(source[row] + col - 1, source[row] + col, len - col + 1); --col; changed = 1;
        } else if (ascii == 8 && row && (int)strlen(source[row - 1]) + len < LINE_LEN) {
            remember(); col = strlen(source[row - 1]); strcat(source[row - 1], source[row]);
            for (i = row; i < lines - 1; ++i) strcpy(source[i], source[i + 1]);
            --row; --lines; changed = 1;
        } else if (scan == 83 && col < len) {
            remember(); memmove(source[row] + col, source[row] + col + 1, len - col); changed = 1;
        } else if (scan == 83 && row < lines - 1 && len + (int)strlen(source[row + 1]) < LINE_LEN) {
            remember(); strcat(source[row], source[row + 1]);
            for (i = row + 1; i < lines - 1; ++i) strcpy(source[i], source[i + 1]);
            --lines; changed = 1;
        }
    } else if (ascii == 13 && lines < MAX_LINES) {
        remember(); strcpy(tail, source[row] + col); source[row][col] = 0;
        for (i = lines; i > row + 1; --i) strcpy(source[i], source[i - 1]);
        ++row; strcpy(source[row], tail); col = 0; ++lines; changed = 1;
    } else if (ascii >= 32 && ascii < 127 && len < LINE_LEN - 1 && !(state & 12)) {
        remember(); memmove(source[row] + col + 1, source[row] + col, len - col + 1);
        source[row][col++] = (char)ascii; changed = 1;
    }
    col = minimum(col, strlen(source[row]));
}
int main(void)
{
    int in[11], out[57], dx, dy, dw, dh, bw, bh, i;
    int msg[8], events, mx, my, mb, ks, key, clicks, top, junk, selected_language;
    FILE *settings;
    if (appl_init() < 0) return 1;
    settings = fopen("C:\\TTBASIC\\STUDIO.CFG", "r");
    if (settings) {
        if (fscanf(settings, "%d", &selected_language) == 1) language_set(selected_language);
        fclose(settings);
    }
    strcpy(status, tr(T_READY));
    vdi = graf_handle(&cw, &ch, &bw, &bh);
    for (i = 0; i < 10; ++i) in[i] = 1;
    in[10] = 2; v_opnvwk(in, &vdi, out);
    if (!vdi) { appl_exit(); return 1; }
    vst_point(vdi, 10, &cw, &ch, &bw, &bh); cw = bw; ch = bh;
    if (cw < 1) cw = 8;
    if (ch < 12) ch = 16;
    vswr_mode(vdi, 1); vsf_interior(vdi, 1); vsf_perimeter(vdi, 0);
    wind_get(0, WF_WORKXYWH, &dx, &dy, &dw, &dh);
    if (dw < 620 || dh < 400) {
        form_alert(1, tr(T_RESOLUTION));
        v_clsvwk(vdi); appl_exit(); return 1;
    }
    menus_init(dw, out[1] + 1);
    window = wind_create(KIND, dx, dy, dw, dh);
    if (window < 0) { menus_close(); v_clsvwk(vdi); appl_exit(); return 1; }
    wind_set(window, WF_NAME, title);
    wind_open(window, dx, dy, dw, dh);
    strcpy(documents[0].path, SOURCE); documents[0].count = 1;
    basic_set_reader(cached_file);
    basic_set_input(runtime_input); basic_set_poll(runtime_poll);
    open_source(); layout(); graf_mouse(ARROW, 0L); done = 0;
    menus_state(0, 0, show_tree, props, show_output);
    while (!done) {
        events = evnt_multi(MU_MESAG | MU_KEYBD | MU_BUTTON, 1, 1, 1,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, msg, 0, 0,
            &mx, &my, &mb, &ks, &key, &clicks);
        if (events & MU_MESAG) {
            if (msg[0] == MN_SELECTED) { action(menus_event(msg[3], msg[4])); refresh(); }
            else if (msg[3] == window) {
                if (msg[0] == WM_REDRAW) redraw(msg[4], msg[5], msg[6], msg[7]);
                else if (msg[0] == WM_CLOSED) done = discard_all();
                else if (msg[0] == WM_TOPPED) wind_set(window, WF_TOP, 0, 0, 0, 0);
                else if (msg[0] == WM_MOVED) {
                    wind_set(window, WF_CURRXYWH, msg[4], msg[5], msg[6], msg[7]); refresh();
                }
            }
        }
        wind_get(window, WF_TOP, &top, &junk, &junk, &junk);
        if ((events & MU_KEYBD) && top == window) { edit(key, ks); refresh(); }
        if ((events & MU_BUTTON) && wind_find(mx, my) == window) {
            if (top != window) wind_set(window, WF_TOP, 0, 0, 0, 0);
            else if (my >= wy + 5 && my < wy + 29) {
                if (mx >= wx + 6 && mx < wx + 100) action(ACT_OPEN);
                else if (mx >= wx + 106 && mx < wx + 218) action(ACT_SAVE);
                else if (mx >= wx + 224 && mx < wx + 360) action(ACT_RUN);
            } else if (show_tree && mx >= wx + 6 && mx < wx + 124 && my >= wy + 65 && my < wy + 89) {
                tree_expanded = !tree_expanded;
            } else if (show_tree && tree_expanded && mx >= wx + 24 && mx < wx + 124 && my >= wy + 119 && my < wy + 141) {
                cache_current(); activate_document(0);
            } else if (show_tree && tree_expanded && mx >= wx + 6 && mx < wx + 124 && my >= wy + 180 && my < wy + wh - 60) {
                i = (my - wy - 180) / 18 + 1; open_import(i);
            } else if (mx >= editor_x + 36 && mx < editor_x + editor_w &&
                       my >= editor_y && my < editor_y + editor_h) {
                row = minimum(lines - 1, first + (my - editor_y) / ch);
                col = minimum(strlen(source[row]), leftcol + (mx - editor_x - 36) / cw);
            }
            refresh();
            evnt_button(1, 1, 0, &mx, &my, &mb, &ks);
        }
    }
    wind_close(window); wind_delete(window); menus_close(); v_clsvwk(vdi); appl_exit(); return 0;
}
