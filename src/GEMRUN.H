#ifndef TT_GEMRUN_H
#define TT_GEMRUN_H
#include "GUI.H"
typedef int (*GemService)(int *message,int key,int state);
void gemrun_init(int vdi,int ide,GemService service);
int gemrun_request(BasicGuiRequest *request);
int gemrun_message(int *message);
void gemrun_top(int handle);
#endif
