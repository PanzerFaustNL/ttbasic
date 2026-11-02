#ifndef TT_GUI_H
#define TT_GUI_H
#define GUI_LIMIT 64
#define GUI_WINDOW 1
#define GUI_BUTTON 2
#define GUI_LABEL 3
#define GUI_TEXTBOX 4
#define GUI_CHECKBOX 5
#define GUI_RESET 1
#define GUI_CREATE 2
#define GUI_GET 3
#define GUI_SET 4
#define GUI_SHOW 5
#define GUI_CLOSE 6
#define GUI_NEXT 7
#define GUI_CLICK 1
#define GUI_CHANGE 2
#define GUI_LOAD 3
#define GUI_CLOSED 4
/* NEXT: 1 event, 0 finished, -1 error. Other operations: 1 success, -1 error. */
typedef struct {
    int operation, kind, id, parent, event;
    int x, y, width, height;
    long number;
    char property[24], text[128], error[80];
} BasicGuiRequest;
typedef int (*BasicGui)(BasicGuiRequest *request);
#endif
