#ifndef TT_DESIGN_H
#define TT_DESIGN_H
#include "FORM.H"
#define DESIGN_APPLY 1
#define DESIGN_CODE 2
#define DESIGN_RUN 3
#define DESIGN_SAVE 4
typedef void (*DesignHost)(int *message);
int designer_edit(FormModel *model,int vdi,int parent,char *filename,DesignHost host,char *handler);
#endif
