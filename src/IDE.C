/* TT BASIC IDE 0.7 - Pure C / GEM / TT Medium. */
#include <aes.h>
#include <vdi.h>
#include <stdio.h>
#include <string.h>
#include "BASIC.H"
#include "MENU.H"
#include "LANG.H"
#include <ctype.h>
#include <tos.h>
#include "PROJECT.H"
#include "COMPLETE.H"
#include "GEMRUN.H"
#include <stdlib.h>
#define DEFAULT_SOURCE "C:\\TTBASIC\\MAIN.BAS"

#define KIND (NAME | CLOSER | MOVER)
static int window, vdi, wx, wy, ww, wh, cw, ch;
static int row, col, first, leftcol, lines = 1, changed, props;
static int editor_x, editor_y, editor_w, editor_h, cols, rows;
static int out_used, error_line, done;
static int output_height = 124;
static Completion completion;
static int completing, completion_selected;
typedef struct { char path[BASIC_PATH];int row; } Breakpoint;
static Breakpoint breakpoints[64];static int breakpoint_count;
static int debug_running,debug_paused,debug_stop,debug_mode,debug_depth,debug_frame,debug_first,debug_reason,debug_skip_stop;
static char debug_path[BASIC_PATH];static int debug_row;
static BasicDebugValue debug_variable;
static int breakpoint_at(char *path,int line);
static void toggle_breakpoint(void);
static void paint_debug_panel(int panel);
static int debug_hook(char *path,int line,int depth,int reason);
static int runtime_service(int *message,int key,int state);
static int show_tree = 1, show_output = 1, tree_expanded = 1;
static int undo_ready, undo_row, undo_col, undo_lines, undo_changed;
static char undo_source[MAX_LINES][LINE_LEN];
static char clipboard[LINE_LEN], query[25];
static int clipboard_ready;
static void action(int command);
static char source[MAX_LINES][LINE_LEN];
static char output[OUT_LINES][OUT_LEN];
static char status[80] = "Gereed | F1 Help  F5 Uitvoeren";
static char title[100] = "TT BASIC Studio 0.7 - MAIN.BAS";
typedef struct {
    char path[BASIC_PATH], text[MAX_LINES][LINE_LEN];
    int count, dirty, row, col, first, left;
} Document;
static Document documents[BASIC_FILES];
static int document_count = 1, active_document;
static Project current_project;
static Document pending_documents[BASIC_FILES];
static char tree_paths[BASIC_FILES*2][BASIC_PATH];
static int tree_count, tree_first, tree_rows;
static void project_status(void);
static void save_settings(void);
static int save_all(void);
static int open_project(char *path);
static void build_tree(void)
{
    int i,j;char *path;
    tree_count=0;
    for(i=0;i<current_project.count+basic_file_count();++i){
        path=i<current_project.count ? current_project.members[i] : basic_file_path(i-current_project.count);
        for(j=0;j<tree_count;++j)if(project_equal(tree_paths[j],path))break;
        if(j==tree_count && tree_count<BASIC_FILES*2)strcpy(tree_paths[tree_count++],path);
    }
}
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
    basic_scan_project(documents[0].text, documents[0].count, current_project.members[0]);
    build_tree();
}
static void activate_document(int index)
{
    Document *d;
    active_document = index; d = &documents[index];
    memcpy(source, d->text, sizeof(source)); lines = d->count;
    changed = d->dirty; row = d->row; col = d->col;
    first = d->first; leftcol = d->left; undo_ready = error_line = 0;
    sprintf(title, "TT BASIC Studio 0.7 - %.8s / %.40s", current_project.name, document_name());
    wind_set(window, WF_NAME, title);
}
static void open_path(char *path);
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
    editor_w = ww - (editor_x - wx) - 6 - (props ? (debug_paused?190:138) : 0);
    output_height = maximum(56 + ch, minimum(output_height, wh - 64 - 3 * ch));
    editor_h = wh - 64 - (show_output ? output_height : 28);
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
static void start_completion(void)
{
    cache_current();
    complete_build(source,lines,row,col,documents[active_document].path,cached_file,&completion);
    completing=completion.count>0;completion_selected=0;
}
static void paint_completion(void)
{
    int x,y,w,h,n,start,i;
    if(!completing)return;
    n=minimum(6,minimum(completion.count,maximum(1,(editor_h-8)/ch-1)));w=minimum(440,editor_w-36);h=(n+1)*ch+8;
    x=editor_x+36+(col-leftcol)*cw;
    x=maximum(editor_x+32,minimum(x,editor_x+editor_w-w));
    y=editor_y+(row-first+1)*ch;
    if(y+h>editor_y+editor_h)y=maximum(editor_y,editor_y+(row-first)*ch-h);
    fill(x,y,w,h,1);fill(x+1,y+1,w-2,h-2,0);
    start=maximum(0,completion_selected-n+1);
    for(i=0;i<n;++i){
        if(start+i==completion_selected)fill(x+2,y+2+i*ch,w-4,ch,4);
        text(x+6,y+2+(i+1)*ch-3,start+i==completion_selected?0:1,
            completion.item[start+i].name,w-12);
    }
    fill(x+2,y+2+n*ch,w-4,ch+3,8);
    text(x+6,y+2+(n+1)*ch-3,1,completion.item[completion_selected].detail,w-12);
}
static void paint(void)
{
    int i, y, line, bottom, panel;
    char buf[100];
    layout(); bottom = wy + wh - output_height;
    fill(wx, wy, ww, wh, 8);
    button(wx + 6, 94, tr(debug_paused?T_CONTINUE:T_LOAD_BUTTON), 0);
    button(wx + 106, 112, tr(debug_paused?T_STEP_BUTTON:T_SAVE_BUTTON), 0);
    button(wx + 224, 136, tr(debug_paused?T_STOP_BUTTON:T_RUN_BUTTON), 1);
    text(wx + 376, wy + 22, 1, "TT BASIC Studio", ww - 388);
    fill(wx, wy + 35, ww, 1, 9);
    if (show_tree) {
        fill(wx + 6, wy + 40, 118, wh - 68, 0);
        heading(wx + 6, wy + 40, 118, tr(T_PROJECT));
        sprintf(buf,"%s %.8s",tree_expanded ? "-" : "+",current_project.name);
        text(wx + 12,wy + 84,1,buf,106);
        if(tree_expanded){
            text(wx + 14,wy + 110,9,tr(T_SOURCE),100);
            tree_rows=maximum(1,(wh-170)/18);
            if(tree_first>=tree_count)tree_first=maximum(0,tree_count-1);
            for(i=0;i<tree_rows && tree_first+i<tree_count;++i){
                int selected;
                panel=tree_first+i;y=wy+119+i*18;
                selected=path_equal(tree_paths[panel],documents[active_document].path);
                if(selected)fill(wx+8,y,114,18,4);
                text(wx+14,y+15,selected?0:1,project_basename(tree_paths[panel]),108);
            }
        }
        sprintf(buf,"%d",tree_count);
        text(wx+42,wy+wh-44,9,buf,60);
        text(wx+14,wy+wh-44,1,"^",16);
        text(wx+102,wy+wh-44,1,"v",16);
    }
    fill(editor_x, wy + 40, editor_w, 24, 0);
    fill(editor_x, wy + 40, minimum(144, editor_w), 3, 4);
    sprintf(buf, "%.60s%s", document_name(), changed ? " *" : "");
    text(editor_x + 8, wy + 58, 1, buf, editor_w - 16);
    fill(editor_x, editor_y, editor_w, editor_h, 0);
    fill(editor_x, editor_y, 32, editor_h, 8);
    for (i = 0; i < rows && first + i < lines; ++i) {
        line = first + i; y = editor_y + i * ch;
        if(debug_paused && line+1==debug_row && path_equal(debug_path,documents[active_document].path))
            fill(editor_x+33,y,editor_w-33,ch,6);
        if (line == row) {
            fill(editor_x + 33, y, editor_w - 33, ch, debug_paused && line+1==debug_row && path_equal(debug_path,documents[active_document].path)?6:8);
            fill(editor_x + 32, y, 2, ch, 4);
        }
        sprintf(buf, "%2d", line + 1);
        text(editor_x + 4, y + ch - 3, error_line == line + 1 ? 2 : 9, buf, 28);
        if(breakpoint_at(documents[active_document].path,line+1)>=0)fill(editor_x+1,y+ch/2-3,6,6,2);
        code_line(line, y);
        if (line == row) fill(editor_x + 36 + (col - leftcol) * cw, y + ch - 2, cw, 2, 4);
    }
    if (props) {
        panel = editor_x + editor_w + 6;
        if(debug_paused)paint_debug_panel(panel);else {
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
    }
    if (show_output) {
        fill(editor_x, bottom, ww - (editor_x - wx) - 6, 6, 9);
        fill(editor_x + (ww - (editor_x - wx) - 6) / 2 - 12, bottom + 2, 24, 2, 1);
        bottom += 6;
        heading(editor_x, bottom, ww - (editor_x - wx) - 6, tr(T_OUTPUT));
        fill(editor_x, bottom + 24, ww - (editor_x - wx) - 6, wy + wh - 26 - (bottom + 24), 0);
        i = maximum(0, out_used - maximum(1, (wy + wh - 26 - bottom - 24) / ch));
        for (y = bottom + 24 + ch - 3; i < out_used && y < wy + wh - 26; ++i, y += ch)
            text(editor_x + 7, y, error_line ? 2 : 1, output[i], ww - (editor_x - wx) - 20);
        if (!out_used) text(editor_x + 7, bottom + 24 + ch - 3, 9, tr(T_RUN_HINT), ww - (editor_x - wx) - 20);
    }
    fill(wx, wy + wh - 22, ww, 22, 4);
    sprintf(buf, "Ln %d : %d | %s", row + 1, col + 1, status);
    text(wx + 8, wy + wh - 6, 0, buf, ww - 16);
    paint_completion();
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
    menus_debug(debug_running,debug_paused);
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
    static char incoming[MAX_LINES][LINE_LEN];int count;
    if(!discard())return;
    if(!project_read_source(documents[active_document].path,incoming,&count)){project_status();return;}
    memcpy(source,incoming,sizeof(source));lines=count;
    row=col=first=leftcol=changed=error_line=undo_ready=0;
    scan_files();sprintf(status,tr(T_OPENED),document_name());
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
static int breakpoint_at(char *path,int line)
{
    int i;for(i=0;i<breakpoint_count;++i)if(breakpoints[i].row==line && path_equal(path,breakpoints[i].path))return i;return -1;
}
static void save_breakpoints(void)
{
    FILE *f;int i;f=fopen("C:\\TTBASIC\\STUDIO.BRK","w");if(!f)return;
    for(i=0;i<breakpoint_count;++i)fprintf(f,"%d %s\n",breakpoints[i].row,breakpoints[i].path);fclose(f);
}
static void load_breakpoints(void)
{
    FILE *f;char line[BASIC_PATH+16],*path;int row,n;
    breakpoint_count=0;f=fopen("C:\\TTBASIC\\STUDIO.BRK","r");if(!f)return;
    while(breakpoint_count<64 && fgets(line,sizeof(line),f)){
        row=atoi(line);path=strchr(line,' ');if(!path || row<1 || row>MAX_LINES)continue;++path;
        n=strlen(path);while(n && (path[n-1]=='\r'||path[n-1]=='\n'))path[--n]=0;
        if(!n || n>=BASIC_PATH || breakpoint_at(path,row)>=0)continue;
        strcpy(breakpoints[breakpoint_count].path,path);breakpoints[breakpoint_count++].row=row;
    }fclose(f);
}
static void toggle_breakpoint(void)
{
    int i;i=breakpoint_at(documents[active_document].path,row+1);
    if(i>=0){--breakpoint_count;memmove(breakpoints+i,breakpoints+i+1,(breakpoint_count-i)*sizeof(Breakpoint));strcpy(status,tr(T_DBG_NOBREAK));}
    else if(breakpoint_count<64){strcpy(breakpoints[breakpoint_count].path,documents[active_document].path);breakpoints[breakpoint_count++].row=row+1;strcpy(status,tr(T_DBG_BREAK));}
    else strcpy(status,tr(T_DBG_BPLIMIT));save_breakpoints();
}
static void debug_location(void)
{
    char name[48],path[BASIC_PATH];int line;
    if(!basic_debug_frame(debug_frame,name,path,&line))return;
    if(!path_equal(path,documents[active_document].path))open_path(path);
    if(path_equal(path,documents[active_document].path)){row=minimum(lines-1,line-1);col=0;}
}
static void paint_debug_panel(int panel)
{
    char name[48],path[BASIC_PATH],buf[64];int line,i,n,y;
    fill(panel,wy+40,184,editor_h+24,0);heading(panel,wy+40,184,tr(T_DBG_VARIABLES));
    text(panel+6,wy+86,9,tr(T_DBG_STACK),108);
    text(panel+122,wy+86,1,"<",16);text(panel+154,wy+86,1,">",16);
    if(basic_debug_frame(debug_frame,name,path,&line)){
        sprintf(buf,"%d %.40s",debug_frame,name);text(panel+6,wy+106,1,buf,172);
        sprintf(buf,"%.16s:%d",project_basename(path),line);text(panel+6,wy+126,9,buf,172);
    }
    n=basic_debug_count(debug_frame);debug_first=minimum(debug_first,maximum(0,n-1));
    for(i=debug_first,y=wy+148;i<n && y+ch+20<editor_y+editor_h;i++,y+=36){
        if(!basic_debug_variable(debug_frame,i,&debug_variable))break;
        text(panel+6,y,1,debug_variable.name,172);text(panel+12,y+17,9,debug_variable.value,166);
    }
    text(panel+8,editor_y+editor_h-4,1,"^",16);text(panel+155,editor_y+editor_h-4,1,"v",16);
}
static void inspect_variable(int index)
{
    char alert[240],clean[128],header[72];int i,n;
    if(!basic_debug_variable(debug_frame,index,&debug_variable))return;
    for(i=0;debug_variable.value[i] && i<127;++i){char c;c=debug_variable.value[i];clean[i]=(c=='|'||c=='['||c==']'||(unsigned char)c<32)?'?':c;}clean[i]=0;
    sprintf(header,"%.36s As %.24s",debug_variable.name,debug_variable.type);
    for(i=0;header[i];++i)if(header[i]=='|'||header[i]=='['||header[i]==']')header[i]='?';
    sprintf(alert,"[1][%s",header);n=strlen(clean);
    for(i=0;i<n;i+=40){strcat(alert,"|");strncat(alert,clean+i,n-i<40?n-i:40);}
    strcat(alert,"][OK]");form_alert(1,alert);
}
static void debug_command(int command,int state)
{
    if(command==ACT_STOP || command==ACT_QUIT){debug_stop=1;debug_paused=0;}
    else if(command==ACT_RUN){debug_mode=0;if(debug_reason==BASIC_DEBUG_ERROR)debug_stop=1;debug_paused=0;}
    else if(command==ACT_STEP || command==ACT_OVER || command==ACT_OUT){
        if(debug_reason==BASIC_DEBUG_ERROR)return;
        debug_mode=command==ACT_OVER?2:command==ACT_OUT?3:1;
        if(command==ACT_STEP && (state & 3))debug_mode=(state & 4)?3:2;
        debug_depth=basic_debug_frames()-1;debug_paused=0;
    }else if(command==ACT_BREAKPOINT)toggle_breakpoint();
    else if(command==ACT_INSPECT)inspect_variable(debug_first);
    else if(command==ACT_TREE || command==ACT_PROPS || command==ACT_OUTPUT)action(command);
}
static int runtime_service(int *m,int key,int state)
{
    int command,scan;scan=(key>>8)&255;
    if(key && (key & 255)==27){debug_stop=1;return 0;}
    if(key && ((state & 4) && scan==25)){debug_mode=1;strcpy(status,tr(T_M_PAUSE));refresh();}
    if(m){
        if(m[0]==MN_SELECTED){
            command=menus_event(m[3],m[4]);
            if(command==ACT_QUIT || command==ACT_STOP){debug_stop=1;return 0;}
            if(command==ACT_PAUSE){debug_mode=1;strcpy(status,tr(T_M_PAUSE));refresh();}
        }else if(m[3]==window){
            if(m[0]==WM_CLOSED){debug_stop=1;return 0;}
            if(m[0]==WM_REDRAW)redraw(m[4],m[5],m[6],m[7]);
            else if(m[0]==WM_TOPPED)wind_set(window,WF_TOP,0,0,0,0);
            else if(m[0]==WM_MOVED){wind_set(window,WF_CURRXYWH,m[4],m[5],m[6],m[7]);refresh();}
        }
    }
    return !debug_stop;
}
static int debug_hook(char *path,int line,int depth,int reason)
{
    int pause,events,m[8],mx,my,mb,state,key,clicks,scan,command,panel,index,previous_top,junk;
    if(debug_stop)return 0;
    if(reason==BASIC_DEBUG_STOP && debug_skip_stop){debug_skip_stop=0;return 1;}
    if(reason==BASIC_DEBUG_LINE)debug_skip_stop=0;
    pause=reason!=BASIC_DEBUG_LINE || breakpoint_at(path,line)>=0 || debug_mode==1 || (debug_mode==2 && depth<=debug_depth) || (debug_mode==3 && depth<debug_depth);
    if(!pause)return 1;
    if(reason==BASIC_DEBUG_ERROR && !strcmp(basic_debug_error(),"Execution cancelled"))return 0;
    debug_skip_stop=reason==BASIC_DEBUG_LINE;debug_reason=reason;debug_paused=1;debug_frame=depth;debug_first=0;
    strcpy(debug_path,path);debug_row=line;props=1;completing=0;
    wind_get(0,WF_TOP,&previous_top,&junk,&junk,&junk);
    output_height=minimum(output_height,wh-264);debug_location();wind_set(window,WF_TOP,0,0,0,0);
    strcpy(status,tr(reason==BASIC_DEBUG_ERROR?T_DBG_ERROR:T_DBG_PAUSED));refresh();
    while(debug_paused && !debug_stop){
        events=evnt_multi(MU_MESAG|MU_KEYBD|MU_BUTTON,1,1,1,0,0,0,0,0,0,0,0,0,0,m,0,0,&mx,&my,&mb,&state,&key,&clicks);
        if(events & MU_MESAG){
            if(m[0]==MN_SELECTED)debug_command(menus_event(m[3],m[4]),0);
            else if(!gemrun_message(m))runtime_service(m,0,0);
        }
        if(events & MU_KEYBD){
            scan=(key>>8)&255;if(scan>=84 && scan<=93)scan-=25;command=0;
            if((key & 255)==27)command=ACT_STOP;
            else if(scan==63)command=ACT_RUN;
            else if(scan==66)command=ACT_STEP;
            else if(scan==67)command=ACT_BREAKPOINT;
            else if((state & 4) && scan==23)command=ACT_INSPECT;
            else if(scan==72)debug_first=maximum(0,debug_first-1);
            else if(scan==80)debug_first=minimum(maximum(0,basic_debug_count(debug_frame)-1),debug_first+1);
            if(command)debug_command(command,state);
        }
        if(events & MU_BUTTON){
            panel=editor_x+editor_w+6;
            if(wind_find(mx,my)==window){
                if(my>=wy+5 && my<wy+29){
                    if(mx<wx+100)debug_command(ACT_RUN,0);
                    else if(mx<wx+218)debug_command(ACT_STEP,0);
                    else if(mx<wx+360)debug_command(ACT_STOP,0);
                }else if(props && mx>=panel){
                    if(my>=wy+68 && my<=wy+92){
                        if(mx<panel+146)debug_frame=maximum(0,debug_frame-1);else debug_frame=minimum(basic_debug_frames()-1,debug_frame+1);
                        debug_first=0;debug_location();
                    }else if(my>=editor_y+editor_h-20){
                        if(mx<panel+80)debug_first=maximum(0,debug_first-1);else debug_first=minimum(maximum(0,basic_debug_count(debug_frame)-1),debug_first+1);
                    }else if(my>=wy+132){index=debug_first+(my-wy-132)/36;inspect_variable(index);}
                }else if(mx>=editor_x && mx<editor_x+editor_w && my>=editor_y && my<editor_y+editor_h){
                    index=first+(my-editor_y)/ch;if(index<lines){row=index;col=0;if(mx<editor_x+32)toggle_breakpoint();}
                }
            }else {index=wind_find(mx,my);if(index>=0)wind_set(index,WF_TOP,0,0,0,0);}
            evnt_button(1,1,0,&mx,&my,&mb,&state);
        }
        if(debug_paused){strcpy(status,tr(reason==BASIC_DEBUG_ERROR?T_DBG_ERROR:T_DBG_PAUSED));refresh();}
    }
    debug_paused=0;menus_debug(debug_running,0);if(!debug_stop && previous_top!=window)gemrun_top(previous_top);return !debug_stop;
}

static int runtime_poll(void)
{
    int message[8],events,mx,my,mb,ks,key,clicks;
    events=evnt_multi(MU_KEYBD|MU_TIMER|MU_MESAG,0,0,0,
        0,0,0,0,0,0,0,0,0,0,message,1,0,&mx,&my,&mb,&ks,&key,&clicks);
    if((events & MU_MESAG) && gemrun_message(message))events&=~MU_MESAG;
    return runtime_service((events & MU_MESAG)?message:0,(events & MU_KEYBD)?key:0,ks);
}
static void run_source(void)
{
    int saved_props;saved_props=props;debug_running=1;debug_paused=debug_stop=debug_depth=debug_skip_stop=0;
    show_output = 1;
    cache_current(); strcpy(status,tr(T_RUNNING)); refresh();
    error_line = basic_run_project(documents[0].text, documents[0].count, current_project.members[0], output, &out_used);
    if(!strcmp(basic_debug_error(),"Execution cancelled"))debug_stop=1;
    debug_running=debug_paused=0;debug_path[0]=0;props=saved_props;debug_mode=0;
    if (error_line) {
        if (!path_equal(documents[active_document].path, basic_error_path()))
            error_line = active_document == 0 ? basic_error_main_line() : -1;
    }
    if (error_line) {
        if (error_line > 0) { row = minimum(lines - 1, error_line - 1); col = 0; }
        strcpy(status, tr(T_BASIC_ERROR));
    }
    else strcpy(status, tr(T_FINISHED));
    if(debug_stop){error_line=0;strcpy(status,tr(T_DBG_STOPPED));
        if(out_used && !strcmp(output[out_used-1],"Program finished."))strcpy(output[out_used-1],"Program stopped.");
    }
    wind_set(window,WF_TOP,0,0,0,0);
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
static void open_path(char *requested)
{
    char path[BASIC_PATH], buf[LINE_LEN + 3];
    FILE *f;
    Document *d;
    int i, n, len, failed;
    if(strlen(requested)>=BASIC_PATH)return;
    strcpy(path,requested); cache_current();
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
static void project_status(void)
{
    sprintf(status,tr(T_PROJECT_ERROR),project_error());
}
static void save_settings(void)
{
    FILE *f;
    f=fopen("C:\\TTBASIC\\STUDIO.CFG","w");
    if(f){fprintf(f,"%d\n%s\n",language_get(),current_project.file);fclose(f);}
}
static int save_all(void)
{
    int old,i;cache_current();old=active_document;
    for(i=0;i<document_count;++i)if(documents[i].dirty){
        activate_document(i);save_source();cache_current();if(changed)return 0;
    }
    activate_document(old);
    if(!project_save(&current_project)){project_status();return 0;}
    save_settings();strcpy(status,tr(T_ALL_SAVED));return 1;
}
static int confirm_switch(void)
{
    int i,count,choice;cache_current();count=0;
    for(i=0;i<document_count;++i)if(documents[i].dirty)++count;
    if(!count)return 1;
    choice=form_alert(1,tr(T_SWITCH_DIRTY));
    if(choice==1)return save_all();return choice==2;
}
static int activate_project(Project *candidate)
{
    int i;
    memset(pending_documents,0,sizeof(pending_documents));
    for(i=0;i<candidate->count;++i){
        strcpy(pending_documents[i].path,candidate->members[i]);
        if(!project_read_source(candidate->members[i],pending_documents[i].text,&pending_documents[i].count)){
            project_status();return 0;
        }
    }
    current_project=*candidate;memcpy(documents,pending_documents,sizeof(documents));
    document_count=candidate->count;active_document=tree_first=out_used=0;
    activate_document(0);scan_files();save_settings();
    sprintf(status,tr(T_PROJECT_OPENED),current_project.name);return 1;
}
static int open_project(char *path)
{
    Project candidate;
    if(!project_load(&candidate,path)){project_status();return 0;}
    return activate_project(&candidate);
}
static int make_directory(char *path) { return Dcreate(path)>=0; }
static int choose_file(char *out,char *filter,char *title)
{
    char selection[16],path[BASIC_PATH],*p;int button;
    if(!project_join(path,current_project.directory,filter)){project_status();return 0;}
    selection[0]=0;button=0;
    if(!fsel_exinput(path,selection,&button,title)||!button||!selection[0])return 0;
    p=project_basename(path);*p=0;
    return project_join(out,path,selection);
}
static void new_project(void)
{
    Project candidate;char name[64];name[0]=0;
    if(!input_dialog(tr(T_PROJECT_NAME),name,sizeof(name)))return;
    if(!confirm_switch())return;
    Dcreate("C:\\TTBASIC\\PROJECTS");
    if(!project_create(&candidate,"C:\\TTBASIC\\PROJECTS",name,make_directory)){project_status();return;}
    activate_project(&candidate);
}
static void new_file(void)
{
    char name[64];name[0]=0;cache_current();
    if(!input_dialog(tr(T_SOURCE_NAME),name,sizeof(name)))return;
    if(!project_new_file(&current_project,name)){project_status();return;}
    build_tree();open_path(current_project.members[current_project.count-1]);
    tree_first=maximum(0,tree_count-tree_rows);save_settings();
    strcpy(status,tr(T_FILE_ADDED));
}
static void add_file(void)
{
    char path[BASIC_PATH];int index;
    if(!choose_file(path,"*.BAS",tr(T_ADD_TITLE)))return;
    if(!project_read_source(path,pending_documents[0].text,&index)){project_status();return;}
    index=project_add_file(&current_project,path);
    if(!index){project_status();return;}
    build_tree();open_path(current_project.members[index-1]);
    tree_first=maximum(0,tree_count-tree_rows);save_settings();strcpy(status,tr(T_FILE_ADDED));
}
static void action(int command)
{
    int i;
    switch (command) {
    case ACT_NEW: new_project();break;
    case ACT_PROJECT_OPEN:
        {
            char path[BASIC_PATH];
            if(choose_file(path,"*.TBP",tr(T_OPEN_PROJECT)) && confirm_switch())open_project(path);
        }
        break;
    case ACT_NEWFILE:new_file();break;
    case ACT_ADDFILE:add_file();break;
    case ACT_SAVEALL:save_all();break;
    case ACT_GEMDEMO:
    case ACT_FLOW:
    case ACT_POLY:
    case ACT_INPUT:
    case ACT_DEMO:
        if(!path_equal(current_project.members[0],DEFAULT_SOURCE)){
            Project legacy;
            if(!confirm_switch())break;
            project_default(&legacy,DEFAULT_SOURCE);
            if(!activate_project(&legacy))break;
        }
        cache_current();
        if (active_document) activate_document(0);
        if (!discard()) break;
        {
            FILE *demo;
            char buf[LINE_LEN];
            int n, len;
            demo = fopen(command==ACT_GEMDEMO ? "C:\\TTBASIC\\GEMDEMO.BAS" : command==ACT_FLOW ? "C:\\TTBASIC\\FLOW.BAS" :
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
    case ACT_RUN: debug_mode=0;run_source(); break;
    case ACT_STEP:debug_mode=1;run_source();break;
    case ACT_BREAKPOINT:toggle_breakpoint();break;
    case ACT_OVER:case ACT_OUT:case ACT_PAUSE:case ACT_STOP:case ACT_INSPECT:break;
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
        save_settings();
        strcpy(status, tr(T_LANG_CHANGED)); break;
    case ACT_ABOUT:
        form_alert(1, tr(T_ABOUT)); break;
    case ACT_KEYS:
        form_alert(1, tr(T_KEYS)); break;
    }
}
static void edit(int key, int state)
{
    int ascii, scan, len, i, was_completing;
    char tail[LINE_LEN];
    ascii = key & 255; scan = (key >> 8) & 255; if(scan>=84 && scan<=93)scan-=25; len = strlen(source[row]);
    was_completing=completing;
    if ((state & 4) && scan==57) { start_completion();return; }
    if (completing) {
        if(scan==72){completion_selected=maximum(0,completion_selected-1);return;}
        if(scan==80){completion_selected=minimum(completion.count-1,completion_selected+1);return;}
        if(ascii==27){completing=0;return;}
        if(ascii==13 || ascii==9){
            i=strlen(completion.item[completion_selected].name);
            if(len-(completion.end-completion.start)+i<LINE_LEN){
                remember();
                memmove(source[row]+completion.start+i,source[row]+completion.end,len-completion.end+1);
                memcpy(source[row]+completion.start,completion.item[completion_selected].name,i);
                col=completion.start+i;changed=1;
            }
            completing=0;return;
        }
        completing=0;
    }
    if (state & 4) {
        switch (scan) {
        case 65:action(ACT_ADDFILE);return;
        case 31: action((state & 3)?ACT_SAVEALL:ACT_SAVE); return;
        case 24: action(ACT_PROJECT_OPEN); return;
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
    if (scan == 65) { action(ACT_NEWFILE); return; }
    if (scan == 66) { action(ACT_STEP); return; }
    if (scan == 67) { action(ACT_BREAKPOINT); return; }
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
    if(!(state & 12) && (ascii=='.' || (was_completing &&
        ((ascii>=32 && ascii<127 && (isalnum(ascii) || ascii=='_')) || ascii==8))))start_completion();
}
int main(void)
{
    int in[11], out[57], dx, dy, dw, dh, bw, bh, i;
    int msg[8], events, mx, my, mb, ks, key, clicks, top, junk, selected_language;
    FILE *settings;
    char last_project[BASIC_PATH];
    last_project[0]=0;
    if (appl_init() < 0) return 1;
    settings = fopen("C:\\TTBASIC\\STUDIO.CFG", "r");
    if (settings) {
        if (fscanf(settings, "%d", &selected_language) == 1) language_set(selected_language);
        fgets(last_project,sizeof(last_project),settings);
        if(fgets(last_project,sizeof(last_project),settings)){
            int n;n=strlen(last_project);while(n&&(last_project[n-1]=='\r'||last_project[n-1]=='\n'))last_project[--n]=0;
        }else last_project[0]=0;
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
    project_default(&current_project,DEFAULT_SOURCE);
    strcpy(documents[0].path,DEFAULT_SOURCE);documents[0].count=1;
    basic_set_reader(cached_file);
    basic_set_input(runtime_input); basic_set_poll(runtime_poll);
    basic_set_debug(debug_hook);basic_set_gui(gemrun_request);gemrun_init(vdi,window,runtime_service);
    if(!last_project[0] || !open_project(last_project)){
        char default_project[BASIC_PATH];
        strcpy(default_project,current_project.file);
        if(!open_project(default_project)){
            project_default(&current_project,DEFAULT_SOURCE);
            strcpy(documents[0].path,DEFAULT_SOURCE);document_count=1;active_document=changed=0;
            open_source();project_save(&current_project);save_settings();
        }
    }
    load_breakpoints();
    layout();graf_mouse(ARROW,0L);done=0;
    menus_state(0, 0, show_tree, props, show_output);
    while (!done) {
        events = evnt_multi(MU_MESAG | MU_KEYBD | MU_BUTTON, 1, 1, 1,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, msg, 0, 0,
            &mx, &my, &mb, &ks, &key, &clicks);
        if (events & MU_MESAG) {
            if (msg[0] == MN_SELECTED) { completing=0; action(menus_event(msg[3], msg[4])); refresh(); }
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
            completing=0;
            if (top != window) wind_set(window, WF_TOP, 0, 0, 0, 0);
            else if (my >= wy + 5 && my < wy + 29) {
                if (mx >= wx + 6 && mx < wx + 100) action(ACT_OPEN);
                else if (mx >= wx + 106 && mx < wx + 218) action(ACT_SAVE);
                else if (mx >= wx + 224 && mx < wx + 360) action(ACT_RUN);
            } else if (show_tree && mx >= wx + 6 && mx < wx + 124 && my >= wy + 65 && my < wy + 89) {
                tree_expanded = !tree_expanded;
            } else if(show_tree && tree_expanded && mx>=wx+6 && mx<wx+124 && my>=wy+119 && my<wy+119+tree_rows*18){
                i=tree_first+(my-wy-119)/18;
                if(i<tree_count)open_path(tree_paths[i]);
            } else if(show_tree && mx>=wx+6 && mx<wx+124 && my>=wy+wh-60 && my<wy+wh-30){
                if(mx<wx+40)tree_first=maximum(0,tree_first-1);
                else if(mx>wx+90)tree_first=minimum(maximum(0,tree_count-tree_rows),tree_first+1);
            } else if (show_output && mx >= editor_x && mx < wx + ww - 6 &&
                       my >= wy + wh - output_height && my < wy + wh - output_height + 6) {
                int drag_x, drag_y, limit_top, limit_bottom;
                limit_top = editor_y + 3 * ch;
                limit_bottom = wy + wh - (56 + ch);
                graf_dragbox(ww - (editor_x - wx) - 6, 6,
                    editor_x, wy + wh - output_height,
                    editor_x, limit_top, ww - (editor_x - wx) - 6,
                    limit_bottom - limit_top + 6, &drag_x, &drag_y);
                output_height = wy + wh - drag_y;
            } else if(mx>=editor_x && mx<editor_x+32 && my>=editor_y && my<editor_y+editor_h){
                i=first+(my-editor_y)/ch;if(i<lines){row=i;toggle_breakpoint();}
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
