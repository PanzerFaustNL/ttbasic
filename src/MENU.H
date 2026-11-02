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
void menus_init(int screen_w, int screen_h);
void menus_close(void);
void menus_language(int language);
int menus_event(int title, int item);
void menus_state(int undo, int paste, int tree, int props, int output);
int search_dialog(char *query);
int input_dialog(char *prompt, char *buffer, int capacity);
#endif
