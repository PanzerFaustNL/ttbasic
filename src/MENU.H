#ifndef TT_MENU_H
#define TT_MENU_H
#define ACT_NONE 0
#define ACT_ABOUT 1
#define ACT_NEW 2
#define ACT_OPEN 3
#define ACT_SAVE 4
#define ACT_QUIT 5
#define ACT_UNDO 6
#define ACT_COPY 7
#define ACT_CUT 8
#define ACT_PASTE 9
#define ACT_FIND 10
#define ACT_NEXT 11
#define ACT_RUN 12
#define ACT_CLEAR 13
#define ACT_TREE 14
#define ACT_PROPS 15
#define ACT_OUTPUT 16
#define ACT_KEYS 17
#define ACT_DEMO 18
#define ACT_LANG_NL 19
#define ACT_LANG_EN 20
#define ACT_LANG_DE 21
#define ACT_FLOW 22
#define ACT_POLY 23
#define ACT_INPUT 24
#define ACT_PROJECT_OPEN 25
#define ACT_NEWFILE 26
#define ACT_ADDFILE 27
#define ACT_SAVEALL 28
#define ACT_STEP 29
#define ACT_OVER 30
#define ACT_OUT 31
#define ACT_BREAKPOINT 32
#define ACT_PAUSE 33
#define ACT_STOP 34
#define ACT_INSPECT 35
#define ACT_GEMDEMO 36
#define ACT_FORM_EDIT 37
#define ACT_FORM_NEW 38
#define ACT_DX7DEMO 39
void menus_modal(int active);
void menus_debug(int running,int paused);
void menus_init(int screen_w, int screen_h);
void menus_close(void);
void menus_language(int language);
int menus_event(int title, int item);
void menus_state(int undo, int paste, int tree, int props, int output);
int search_dialog(char *query);
int input_dialog(char *prompt, char *buffer, int capacity);
#endif
