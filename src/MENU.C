/* Native AES menus and find dialog: no resource file required. */
#include <aes.h>
#include <string.h>
#include "MENU.H"
#include "LANG.H"
#define MAX_OBJS 112
static OBJECT menu[MAX_OBJS];
static int actions[MAX_OBJS], items[64], used;
static int screen_width, screen_height;
static char *titles[6];
static int title_width[] = { 10, 9, 12, 10, 9, 8 };
static void object(OBJECT *tree, int n, int type, int x, int y, int w, int h)
{
    memset(&tree[n], 0, sizeof(OBJECT));
    tree[n].ob_next = tree[n].ob_head = tree[n].ob_tail = -1;
    tree[n].ob_type = type; tree[n].ob_x = x; tree[n].ob_y = y;
    tree[n].ob_width = w; tree[n].ob_height = h;
    if (type == G_BOX) tree[n].ob_spec.index = 0x00ff1100L;
}
static void entry(int parent, int row, char *label, int action, int width)
{
    int n;
    n = used++;
    object(menu, n, G_STRING, 0, row * 16, width, 16);
    menu[n].ob_spec.free_string = label;
    if (!action) menu[n].ob_state = DISABLED;
    else { actions[n] = action; items[action] = n; }
    objc_add(menu, parent, n);
}
void menus_init(int screen_w, int screen_h)
{
    int i, x, w, box, title, row;
    screen_width = screen_w; screen_height = screen_h;
    memset(menu, 0, sizeof(menu)); memset(actions, 0, sizeof(actions)); memset(items, 0, sizeof(items));
    titles[0] = " TT BASIC "; titles[1] = tr(T_FILE); titles[2] = tr(T_EDIT);
    titles[3] = tr(T_PROJECT_MENU); titles[4] = tr(T_VIEW); titles[5] = tr(T_HELP);
    object(menu, 0, G_IBOX, 0, 0, screen_w, screen_h);
    object(menu, 1, G_BOX, 0, 0, screen_w, 19);
    object(menu, 2, G_IBOX, 16, 0, screen_w - 16, 19);
    objc_add(menu, 0, 1); objc_add(menu, 1, 2);
    x = 0;
    for (i = 0; i < 6; ++i) {
        title = 3 + i; w = title_width[i] * 8;
        object(menu, title, G_TITLE, x, 0, w, 19);
        menu[title].ob_spec.free_string = titles[i];
        objc_add(menu, 2, title); x += w;
    }
    menu[2].ob_width = x;
    object(menu, 9, G_IBOX, 0, 19, screen_w, screen_h - 19);
    objc_add(menu, 0, 9); used = 10; x = 16;
    for (i = 0; i < 6; ++i) {
        box = used++; w = (i == 2 || i == 3) ? 304 : 280;
        object(menu, box, G_BOX, x, 0, w, 16);
        objc_add(menu, 9, box); row = 0;
#define ITEM(s,a) entry(box, row++, s, a, w)
        if (i == 0) {
            ITEM(tr(T_M_ABOUT), ACT_ABOUT);
            ITEM("-----------------------------", 0);
            ITEM("  Accessoire 1", 0); ITEM("  Accessoire 2", 0);
            ITEM("  Accessoire 3", 0); ITEM("  Accessoire 4", 0);
            ITEM("  Accessoire 5", 0); ITEM("  Accessoire 6", 0);
        } else if (i == 1) {
            ITEM(tr(T_M_NEW),ACT_NEW);
            ITEM(tr(T_M_PROJECT_OPEN),ACT_PROJECT_OPEN);
            ITEM("-----------------------------",0);
            ITEM(tr(T_M_NEWFILE),ACT_NEWFILE);ITEM(tr(T_M_FORM_NEW),ACT_FORM_NEW);
            ITEM(tr(T_M_ADDFILE),ACT_ADDFILE);
            ITEM("-----------------------------",0);
            ITEM(tr(T_M_SAVE),ACT_SAVE);
            ITEM(tr(T_M_SAVEALL),ACT_SAVEALL);
            ITEM(tr(T_M_OPEN),ACT_OPEN);
            ITEM("-----------------------------",0);
            ITEM(tr(T_M_QUIT),ACT_QUIT);
        } else if (i == 2) {
            ITEM(tr(T_M_UNDO), ACT_UNDO);
            ITEM("-------------------------------", 0);
            ITEM(tr(T_M_COPY), ACT_COPY);
            ITEM(tr(T_M_CUT), ACT_CUT);
            ITEM(tr(T_M_PASTE), ACT_PASTE);
            ITEM("-------------------------------", 0);
            ITEM(tr(T_M_FIND), ACT_FIND);
            ITEM(tr(T_M_NEXT), ACT_NEXT);
        } else if (i == 3) {
            ITEM(tr(T_M_FORM_EDIT),ACT_FORM_EDIT);ITEM(tr(T_M_RUN), ACT_RUN);
            ITEM(tr(T_M_STEP),ACT_STEP);ITEM(tr(T_M_OVER),ACT_OVER);ITEM(tr(T_M_OUT),ACT_OUT);
            ITEM(tr(T_M_BREAKPOINT),ACT_BREAKPOINT);ITEM(tr(T_M_PAUSE),ACT_PAUSE);
            ITEM(tr(T_M_STOP),ACT_STOP);ITEM(tr(T_M_INSPECT),ACT_INSPECT);
            ITEM(tr(T_M_CLEAR), ACT_CLEAR);
            ITEM("-----------------------------", 0);
            ITEM(tr(T_M_DEMO), ACT_DEMO);
            ITEM(tr(T_M_FLOW), ACT_FLOW);
            ITEM(tr(T_M_POLY), ACT_POLY);
            ITEM(tr(T_M_INPUT), ACT_INPUT);ITEM(tr(T_M_GEMDEMO),ACT_GEMDEMO);ITEM(tr(T_M_DX7DEMO),ACT_DX7DEMO);
        } else if (i == 4) {
            ITEM(tr(T_M_TREE), ACT_TREE);
            ITEM(tr(T_M_PROPS), ACT_PROPS);
            ITEM(tr(T_M_OUTPUT), ACT_OUTPUT);
            ITEM("-----------------------------", 0);
            ITEM("  Nederlands", ACT_LANG_NL);
            ITEM("  English", ACT_LANG_EN);
            ITEM("  Deutsch", ACT_LANG_DE);
        } else {
            ITEM(tr(T_M_KEYS), ACT_KEYS);
            ITEM(tr(T_M_ABOUT), ACT_ABOUT);
        }
#undef ITEM
        menu[box].ob_height = row * 16;
        if (x + w > screen_w) menu[box].ob_x = screen_w - w;
        x += title_width[i] * 8;
    }
    menu[used - 1].ob_flags |= LASTOB;
    menu_bar(menu, 1);
}
void menus_close(void) { menu_bar(menu, 0); }
void menus_language(int language)
{
    menu_bar(menu, 0); language_set(language); menus_init(screen_width, screen_height);
}
int menus_event(int title, int item)
{
    menu_tnormal(menu, title, 1);
    if (item < 0 || item >= used || (menu[item].ob_state & DISABLED)) return ACT_NONE;
    return actions[item];
}
void menus_state(int undo, int paste, int tree, int props, int output)
{
    menu_ienable(menu, items[ACT_UNDO], undo);
    menu_ienable(menu, items[ACT_PASTE], paste);
    menu_icheck(menu, items[ACT_TREE], tree);
    menu_icheck(menu, items[ACT_PROPS], props);
    menu_icheck(menu, items[ACT_OUTPUT], output);
    menu_icheck(menu, items[ACT_LANG_NL], language_get() == 0);
    menu_icheck(menu, items[ACT_LANG_EN], language_get() == 1);
    menu_icheck(menu, items[ACT_LANG_DE], language_get() == 2);
}
int search_dialog(char *query)
{
    OBJECT dialog[5];
    TEDINFO field;
    char input[25], pattern[] = "________________________";
    int x, y, w, h, selected;
    strcpy(input, query); memset(&field, 0, sizeof(field));
    field.te_ptext = input; field.te_ptmplt = pattern;
    field.te_pvalid = "X"; field.te_font = IBM; field.te_just = TE_LEFT;
    field.te_color = 0x1180; field.te_thickness = -1;
    field.te_txtlen = 25; field.te_tmplen = 25;
    object(dialog, 0, G_BOX, 0, 0, 304, 132);
    dialog[0].ob_state = OUTLINED;
    object(dialog, 1, G_STRING, 16, 12, 272, 16);
    dialog[1].ob_spec.free_string = tr(T_FIND_TITLE);
    object(dialog, 2, G_FTEXT, 20, 47, 264, 16);
    dialog[2].ob_spec.tedinfo = &field; dialog[2].ob_flags = EDITABLE;
    object(dialog, 3, G_BUTTON, 16, 94, 120, 22);
    dialog[3].ob_spec.free_string = tr(T_FIND_BUTTON);
    dialog[3].ob_flags = SELECTABLE | EXIT | DEFAULT;
    object(dialog, 4, G_BUTTON, 158, 94, 128, 22);
    dialog[4].ob_spec.free_string = tr(T_CANCEL);
    dialog[4].ob_flags = SELECTABLE | EXIT | LASTOB;
    objc_add(dialog, 0, 1); objc_add(dialog, 0, 2);
    objc_add(dialog, 0, 3); objc_add(dialog, 0, 4);
    form_center(dialog, &x, &y, &w, &h);
    form_dial(0, 0, 0, 0, 0, x, y, w, h);
    objc_draw(dialog, 0, MAX_DEPTH, x, y, w, h);
    selected = form_do(dialog, 2) & 0x7fff;
    form_dial(3, 0, 0, 0, 0, x, y, w, h);
    if (selected != 3 || !input[0]) return 0;
    strcpy(query, input); return 1;
}

int input_dialog(char *prompt, char *buffer, int capacity)
{
    OBJECT dialog[6]; TEDINFO field;
    char input[64],pattern[64],label[64];
    int x,y,w,h,selected,limit;
    limit=capacity-1;if(limit>63)limit=63;if(limit<1)return 0;
    memset(input,0,sizeof(input));strncpy(input,buffer,limit);
    memset(pattern,'_',limit);pattern[limit]=0;
    strncpy(label,prompt,63); label[63]=0; memset(&field,0,sizeof(field));
    field.te_ptext=input; field.te_ptmplt=pattern; field.te_pvalid="X";
    field.te_font=IBM; field.te_just=TE_LEFT; field.te_color=0x1180;
    field.te_thickness=-1; field.te_txtlen=limit+1; field.te_tmplen=limit+1;
    object(dialog,0,G_BOX,0,0,560,160); dialog[0].ob_state=OUTLINED;
    object(dialog,1,G_STRING,16,12,528,16); dialog[1].ob_spec.free_string=tr(T_INPUT_TITLE);
    object(dialog,2,G_STRING,16,44,528,16); dialog[2].ob_spec.free_string=label;
    object(dialog,3,G_FTEXT,24,76,512,16); dialog[3].ob_spec.tedinfo=&field; dialog[3].ob_flags=EDITABLE;
    object(dialog,4,G_BUTTON,128,120,112,24); dialog[4].ob_spec.free_string=tr(T_OK);
    dialog[4].ob_flags=SELECTABLE|EXIT|DEFAULT;
    object(dialog,5,G_BUTTON,304,120,128,24); dialog[5].ob_spec.free_string=tr(T_CANCEL);
    dialog[5].ob_flags=SELECTABLE|EXIT|LASTOB;
    objc_add(dialog,0,1); objc_add(dialog,0,2); objc_add(dialog,0,3); objc_add(dialog,0,4); objc_add(dialog,0,5);
    form_center(dialog,&x,&y,&w,&h); form_dial(0,0,0,0,0,x,y,w,h);
    objc_draw(dialog,0,MAX_DEPTH,x,y,w,h); selected=form_do(dialog,3)&0x7fff;
    form_dial(3,0,0,0,0,x,y,w,h);
    if (selected!=4) return 0;
    if (capacity>0) { strncpy(buffer,input,capacity-1); buffer[capacity-1]=0; }
    return 1;
}

void menus_debug(int running,int paused)
{
    int i,enabled;
    for(i=1;i<=ACT_DX7DEMO;++i)if(items[i]){
        enabled=1;
        if(running)enabled=i==ACT_RUN || i==ACT_QUIT || i==ACT_STOP || i==ACT_PAUSE || i==ACT_TREE || i==ACT_OUTPUT || i==ACT_PROPS || i==ACT_BREAKPOINT || i==ACT_INSPECT || i==ACT_STEP || i==ACT_OVER || i==ACT_OUT;
        if(i==ACT_STOP || i==ACT_PAUSE)enabled=running;
        if(i==ACT_INSPECT || i==ACT_OVER || i==ACT_OUT)enabled=paused;
        if(running && !paused && (i==ACT_RUN || i==ACT_STEP))enabled=0;
        if(!running && (i==ACT_UNDO || i==ACT_PASTE))continue;
        menu_ienable(menu,items[i],enabled);
    }
}

void menus_modal(int active)
{
    int i;if(!active){menus_debug(0,0);return;}
    for(i=1;i<=ACT_DX7DEMO;++i)if(items[i])menu_ienable(menu,items[i],0);
}

/* AES has no key-up event. Poll modifiers only while this shortcut is
   latched, and rearm only after the queued key events have drained. */
static int form_key_latched;
int menus_form_key(int key,int state)
{
    if(((key>>8)&255)!=32 || (!(state&4) && (key&255)!=4))return 0;
    if(form_key_latched)return -1;
    form_key_latched=1;return 1;
}
int menus_form_key_pending(void){return form_key_latched;}
void menus_form_key_idle(int state){if(!(state&4))form_key_latched=0;}
