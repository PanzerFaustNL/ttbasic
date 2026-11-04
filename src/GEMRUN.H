#ifndef TT_GEMRUN_H
#define TT_GEMRUN_H
#include "GUI.H"
/* Result: 0 cancels, 1 passes the key, 2 consumes it.
   button is zero unless a mouse-button event was received. */
typedef int (*GemService)(int *message,int key,int state,int mx,int my,int button);
void gemrun_init(int vdi,int ide,GemService service);
int gemrun_request(BasicGuiRequest *request);
int gemrun_message(int *message);
void gemrun_top(int handle);
void gemrun_code(void);
void gemrun_switch_view(void);
#endif
