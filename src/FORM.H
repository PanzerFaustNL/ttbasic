#ifndef TT_FORM_H
#define TT_FORM_H
#include "BASIC.H"
#define FORM_CONTROLS 16
#define FORM_NAME 24
#define FORM_TEXT 64
#define FORM_NAME_PROP 0
#define FORM_TEXT_PROP 1
#define FORM_X 2
#define FORM_Y 3
#define FORM_WIDTH 4
#define FORM_HEIGHT 5
#define FORM_HANDLER 6
#define FORM_CLOSED 7
#define FORM_ENABLED 8
#define FORM_VISIBLE 9
#define FORM_CHECKED 10
typedef struct {
    int kind,x,y,width,height,enabled,visible,checked;
    char name[FORM_NAME],text[FORM_TEXT],handler[FORM_NAME];
} FormControl;
typedef struct {
    char name[FORM_NAME],text[FORM_TEXT],load[FORM_NAME],closed[FORM_NAME];
    int width,height,next_id,count;
    FormControl control[FORM_CONTROLS];
} FormModel;
void form_default(FormModel *model);
char *form_last_error(void);
char *form_kind(int kind);
char *form_event(int kind);
int form_validate(FormModel *model);
int form_add(FormModel *model,int kind,int x,int y);
int form_delete(FormModel *model,int index);
int form_set(FormModel *model,int index,int property,char *value);
int form_bounds(FormModel *model,int index,int x,int y,int width,int height);
/* read: 1 managed form, 0 no form, -1 invalid metadata (never execute source). */
int form_read(BasicSource source,int count,FormModel *model);
/* Preserve all user lines outside the managed region, add missing event Subs. */
int form_generate(FormModel *model,BasicSource source,int count,BasicSource output,int *used);
int form_handler_line(BasicSource source,int count,char *name);
#endif
