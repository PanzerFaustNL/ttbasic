/* BASIC-owned AES windows. No nested application loop or resource file. */
#include <aes.h>
#include <vdi.h>
#include <stdio.h>
#include <string.h>
#include "GEMRUN.H"
#define WINDOWS 4
#define WKIND (NAME|CLOSER|MOVER)
typedef struct {
    int used,kind,parent,x,y,w,h,enabled,visible,checked,object,capacity;
    char text[128],pattern[65];TEDINFO field;
} Control;
typedef struct {
    int used,id,handle,open,x,y,w,h,last_focus;
    OBJECT tree[GUI_LIMIT+1];int ids[GUI_LIMIT+1],count;
} Form;
static Control controls[GUI_LIMIT];
static Form forms[WINDOWS];
static int focus=-1,edit_index,events[32][2],head,tail,overflow;
static GemService host_service;
static int host_window,last_form;
static Form *parent_form(int id)
{
    int i;if(id<0 || id>=GUI_LIMIT || !controls[id].used)return 0;
    if(controls[id].kind!=GUI_WINDOW)id=controls[id].parent;
    for(i=0;i<WINDOWS;++i)if(forms[i].used && forms[i].id==id)return &forms[i];return 0;
}
static void queue_event(int id,int event)
{
    int next;next=(tail+1)%32;if(next==head){overflow=1;return;}
    events[tail][0]=id;events[tail][1]=event;tail=next;
}
static int error(BasicGuiRequest *r,char *text){strncpy(r->error,text,79);r->error[79]=0;return -1;}
static void object(OBJECT *tree,int n,int type,int x,int y,int w,int h)
{
    memset(&tree[n],0,sizeof(OBJECT));tree[n].ob_next=tree[n].ob_head=tree[n].ob_tail=-1;
    tree[n].ob_type=type;tree[n].ob_x=x;tree[n].ob_y=y;tree[n].ob_width=w;tree[n].ob_height=h;
    if(type==G_BOX)tree[n].ob_spec.index=0x00ff1100L;
}
static void focus_control(int id)
{
    Form *f;
    if(focus>=0){f=parent_form(focus);if(f && f->open)objc_edit(f->tree,controls[focus].object,0,&edit_index,ED_END);}
    focus=-1;
    if(id<0 || !controls[id].used || controls[id].kind!=GUI_TEXTBOX || !controls[id].enabled || !controls[id].visible)return;
    f=parent_form(id);if(!f || !f->open)return;
    focus=id;f->last_focus=id;edit_index=strlen(controls[id].text);objc_edit(f->tree,controls[id].object,0,&edit_index,ED_INIT);
}
static void build(Form *f)
{
    int i,n,wx,wy,ww,wh;Control *c;
    wind_get(f->handle,WF_WORKXYWH,&wx,&wy,&ww,&wh);
    object(f->tree,0,G_BOX,wx,wy,ww,wh);f->count=1;f->ids[0]=f->id;
    for(i=0;i<GUI_LIMIT;++i)if(controls[i].used && controls[i].kind!=GUI_WINDOW && controls[i].parent==f->id){
        c=&controls[i];n=f->count++;c->object=n;f->ids[n]=i;
        object(f->tree,n,c->kind==GUI_LABEL?G_STRING:c->kind==GUI_TEXTBOX?G_FTEXT:G_BUTTON,c->x,c->y,c->w,c->h);
        if(c->kind==GUI_TEXTBOX){
            memset(&c->field,0,sizeof(c->field));c->field.te_ptext=c->text;c->field.te_ptmplt=c->pattern;c->field.te_pvalid="X";
            c->field.te_font=IBM;c->field.te_just=TE_LEFT;c->field.te_color=0x1180;c->field.te_thickness=-1;
            c->field.te_txtlen=c->capacity+1;c->field.te_tmplen=c->capacity+1;
            f->tree[n].ob_spec.tedinfo=&c->field;f->tree[n].ob_flags=EDITABLE;
        }else{
            f->tree[n].ob_spec.free_string=c->text;
            if(c->kind==GUI_BUTTON)f->tree[n].ob_flags=SELECTABLE|EXIT;
            if(c->kind==GUI_CHECKBOX)f->tree[n].ob_flags=SELECTABLE;
        }
        if(!c->visible)f->tree[n].ob_flags|=HIDETREE;
        if(!c->enabled)f->tree[n].ob_state|=DISABLED;
        if(c->checked)f->tree[n].ob_state|=SELECTED;
        objc_add(f->tree,0,n);
    }
    f->tree[f->count-1].ob_flags|=LASTOB;
}
static void paint(Form *f,int x,int y,int w,int h)
{
    int rx,ry,rw,rh,x1,y1,x2,y2,old_focus;
    if(!f || !f->open)return;
    old_focus=focus;if(parent_form(focus)==f)focus_control(-1);else old_focus=-1;
    build(f);wind_update(BEG_UPDATE);graf_mouse(M_OFF,0L);
    wind_get(f->handle,WF_FIRSTXYWH,&rx,&ry,&rw,&rh);
    while(rw && rh){
        x1=rx>x?rx:x;y1=ry>y?ry:y;x2=rx+rw<x+w?rx+rw:x+w;y2=ry+rh<y+h?ry+rh:y+h;
        if(x2>x1 && y2>y1)objc_draw(f->tree,0,8,x1,y1,x2-x1,y2-y1);
        wind_get(f->handle,WF_NEXTXYWH,&rx,&ry,&rw,&rh);
    }
    graf_mouse(M_ON,0L);wind_update(END_UPDATE);
    if(old_focus>=0)focus_control(old_focus);
}
static void repaint(Form *f)
{
    int x,y,w,h;if(!f || !f->open)return;wind_get(f->handle,WF_WORKXYWH,&x,&y,&w,&h);paint(f,x,y,w,h);
}
static void close_form(Form *f,int event)
{
    if(!f || !f->open)return;
    if(parent_form(focus)==f)focus_control(-1);
    wind_close(f->handle);f->open=0;controls[f->id].visible=0;
    if(event)queue_event(f->id,GUI_CLOSED);
}
void gemrun_init(int vdi,int ide,GemService service){(void)vdi;host_window=ide;last_form=-1;host_service=service;}
int gemrun_message(int *m)
{
    int i;Form *f;
    for(i=0;i<WINDOWS;++i)if(forms[i].used && forms[i].handle==m[3]){
        f=&forms[i];
        if(m[0]==WM_REDRAW)paint(f,m[4],m[5],m[6],m[7]);
        else if(m[0]==WM_TOPPED)gemrun_top(f->handle);
        else if(m[0]==WM_MOVED){f->x=m[4];f->y=m[5];wind_set(f->handle,WF_CURRXYWH,m[4],m[5],m[6],m[7]);repaint(f);}
        else if(m[0]==WM_CLOSED)close_form(f,1);
        else return 0;
        return 1;
    }
    return 0;
}
static void next_focus(Form *f,int reverse)
{
    int i,start,n,id;start=focus>=0?controls[focus].object:0;
    for(i=1;i<f->count;++i){n=(start+(reverse?-i:i)+f->count)%f->count;id=f->ids[n];
        if(controls[id].kind==GUI_TEXTBOX && controls[id].visible && controls[id].enabled){focus_control(id);return;}}
}
static int next_event(BasicGuiRequest *r)
{
    int i,open,m[8],e,mx,my,mb,ks,key,clicks,top,junk,id,n,inside,handled;Form *f;Control *c;char before[128];
    while(1){
        if(overflow)return error(r,"GEM event queue is full");
        if(head!=tail){r->id=events[head][0];r->event=events[head][1];head=(head+1)%32;return 1;}
        open=0;for(i=0;i<WINDOWS;++i)if(forms[i].used && forms[i].open)++open;if(!open)return 0;
        memset(m,0,sizeof(m));key=ks=0;
        e=evnt_multi(MU_MESAG|MU_KEYBD|MU_BUTTON|MU_TIMER,1,1,1,0,0,0,0,0,0,0,0,0,0,m,20,0,&mx,&my,&mb,&ks,&key,&clicks);
        if((e & MU_KEYBD) && (key & 255)==27)return error(r,"Execution cancelled");
        handled=host_service?host_service((e & MU_MESAG)?m:0,(e & MU_KEYBD)?key:0,ks,mx,my,(e & MU_BUTTON)?mb:0):1;
        if(!handled)return error(r,"Execution cancelled");
        if(handled>1)e&=~MU_KEYBD;
        if(e & MU_MESAG)gemrun_message(m);
        wind_get(0,WF_TOP,&top,&junk,&junk,&junk);f=0;
        for(i=0;i<WINDOWS;++i)if(forms[i].used && forms[i].open && forms[i].handle==top)f=&forms[i];
        if((e & MU_KEYBD) && f){
            if((key & 255)==9)next_focus(f,ks & 3);
            else if((key & 255)==13){
                for(i=1;i<f->count;++i){id=f->ids[i];if(controls[id].kind==GUI_BUTTON && controls[id].enabled && controls[id].visible){queue_event(id,GUI_CLICK);break;}}
            }else if(focus>=0 && parent_form(focus)==f){
                strcpy(before,controls[focus].text);objc_edit(f->tree,controls[focus].object,key,&edit_index,ED_CHAR);
                if(strcmp(before,controls[focus].text))queue_event(focus,GUI_CHANGE);
            }
        }
        if(e & MU_BUTTON){
            i=wind_find(mx,my);f=0;for(n=0;n<WINDOWS;++n)if(forms[n].used && forms[n].open && forms[n].handle==i)f=&forms[n];
            if(f){
                if(top!=f->handle)gemrun_top(f->handle);
                else{
                    n=objc_find(f->tree,0,8,mx,my);id=n>0 && n<f->count?f->ids[n]:-1;
                    if(id>=0 && controls[id].enabled && controls[id].visible){
                        c=&controls[id];
                        if(c->kind==GUI_TEXTBOX)focus_control(id);
                        else if(c->kind==GUI_BUTTON || c->kind==GUI_CHECKBOX){
                            if(c->kind==GUI_BUTTON){f->tree[n].ob_state|=SELECTED;objc_draw(f->tree,n,0,0,0,640,480);}
                            evnt_button(1,1,0,&mx,&my,&mb,&ks);inside=objc_find(f->tree,0,8,mx,my)==n;
                            if(c->kind==GUI_BUTTON)f->tree[n].ob_state&=~SELECTED;
                            if(inside){if(c->kind==GUI_CHECKBOX)c->checked=!c->checked;queue_event(id,GUI_CLICK);}
                            repaint(f);
                        }
                    }
                }
            }
            if(mb & 1)evnt_button(1,1,0,&mx,&my,&mb,&ks);
        }
    }
}
int gemrun_request(BasicGuiRequest *r)
{
    int i,x,y,w,h,bx,by,bw,bh,length,saved_focus;Form *f;Control *c;char old[128];
    if(r->operation==GUI_RESET){
        focus_control(-1);for(i=0;i<WINDOWS;++i)if(forms[i].used){close_form(&forms[i],0);wind_delete(forms[i].handle);}
        memset(forms,0,sizeof(forms));memset(controls,0,sizeof(controls));head=tail=overflow=0;
        return 1;
    }
    if(r->operation==GUI_NEXT)return next_event(r);
    if(r->id<0 || r->id>=GUI_LIMIT)return error(r,"Invalid GUI object");c=&controls[r->id];
    if(r->operation==GUI_CREATE){
        if(c->used)return error(r,"GUI object already exists");
        if(strlen(r->text)>63)return error(r,"GEM text exceeds 63 characters");
        if(r->kind==GUI_WINDOW){
            for(i=0;i<WINDOWS && forms[i].used;++i){}if(i==WINDOWS)return error(r,"Maximum four BASIC windows");f=&forms[i];
            wind_get(0,WF_WORKXYWH,&x,&y,&w,&h);wind_calc(WC_BORDER,WKIND,0,0,r->width,r->height,&bx,&by,&bw,&bh);
            if(bw>w || bh>h)return error(r,"Window is larger than the desktop");
            f->x=x+(w-bw)/2;f->y=y+(h-bh)/2;f->w=bw;f->h=bh;
            f->handle=wind_create(WKIND,x,y,w,h);if(f->handle<0)return error(r,"Cannot create GEM window");
            f->used=1;f->id=r->id;f->last_focus=-1;
        }else{
            f=parent_form(r->parent);if(!f || controls[r->parent].kind!=GUI_WINDOW)return error(r,"Control requires a Window parent");
            if(r->width<16 || r->height<16 || r->x+r->width>controls[r->parent].w || r->y+r->height>controls[r->parent].h)return error(r,"Control does not fit inside Window");
        }
        c->used=1;c->kind=r->kind;c->parent=r->parent;c->x=r->x;c->y=r->y;c->w=r->width;c->h=r->height;c->enabled=1;c->visible=r->kind!=GUI_WINDOW;
        c->capacity=r->kind==GUI_TEXTBOX?(r->width-8)/8:63;if(c->capacity>63)c->capacity=63;
        if(strlen(r->text)>(unsigned)c->capacity){c->used=0;return error(r,"Text exceeds control capacity");}
        strcpy(c->text,r->text);memset(c->pattern,'_',c->capacity);c->pattern[c->capacity]=0;
        if(r->kind==GUI_WINDOW)wind_set(f->handle,WF_NAME,c->text);else if(f->open)repaint(f);
        return 1;
    }
    if(!c->used)return error(r,"GUI object does not exist");f=parent_form(r->id);
    if(r->operation==GUI_SHOW){
        if(!f || c->kind!=GUI_WINDOW)return error(r,"Show requires a Window");
        if(!f->open){wind_open(f->handle,f->x,f->y,f->w,f->h);f->open=1;c->visible=1;repaint(f);next_focus(f,0);queue_event(r->id,GUI_LOAD);}
        else wind_set(f->handle,WF_TOP,0,0,0,0);return 1;
    }
    if(r->operation==GUI_CLOSE){close_form(f,1);return 1;}
    if(r->operation!=GUI_GET && r->operation!=GUI_SET)return error(r,"Unknown GEM operation");
    if(r->operation==GUI_GET){
        if(!strcmp(r->property,"TEXT"))strcpy(r->text,c->text);
        else r->number=(!strcmp(r->property,"ENABLED")?c->enabled:!strcmp(r->property,"VISIBLE")?c->visible:c->checked)?-1:0;
        return 1;
    }
    saved_focus=focus;
    if(!strcmp(r->property,"TEXT")){
        length=strlen(r->text);if(length>c->capacity)return error(r,"Text exceeds control capacity");
        strcpy(old,c->text);if(focus==r->id)focus_control(-1);strcpy(c->text,r->text);
        if(c->kind==GUI_WINDOW)wind_set(f->handle,WF_NAME,c->text);
        if(c->kind==GUI_TEXTBOX && strcmp(old,c->text))queue_event(r->id,GUI_CHANGE);
    }else if(!strcmp(r->property,"VISIBLE")){
        if(c->kind==GUI_WINDOW){if(r->number){r->operation=GUI_SHOW;return gemrun_request(r);}close_form(f,1);return 1;}
        c->visible=r->number!=0;if(!c->visible && focus==r->id)focus_control(-1);
    }else if(!strcmp(r->property,"ENABLED")){c->enabled=r->number!=0;if(!c->enabled && focus==r->id)focus_control(-1);}
    else if(!strcmp(r->property,"CHECKED"))c->checked=r->number!=0;
    else return error(r,"Unknown GEM property");
    repaint(f);if(focus<0 && saved_focus>=0)focus_control(saved_focus);return 1;
}

void gemrun_top(int handle)
{
    int i;for(i=0;i<WINDOWS;++i)if(forms[i].used && forms[i].open && forms[i].handle==handle){
        focus_control(-1);wind_set(handle,WF_TOP,0,0,0,0);last_form=handle;repaint(&forms[i]);
        if(forms[i].last_focus>=0)focus_control(forms[i].last_focus);else next_focus(&forms[i],0);return;
    }
}

void gemrun_code(void)
{
    Form *f;f=parent_form(focus);if(f && f->open)last_form=f->handle;
    focus_control(-1);wind_set(host_window,WF_TOP,0,0,0,0);
}
void gemrun_switch_view(void)
{
    int top,junk,i;wind_get(0,WF_TOP,&top,&junk,&junk,&junk);
    if(top!=host_window){gemrun_code();return;}
    for(i=0;i<WINDOWS;++i)if(forms[i].used && forms[i].open && forms[i].handle==last_form){gemrun_top(last_form);return;}
    for(i=0;i<WINDOWS;++i)if(forms[i].used && forms[i].open){gemrun_top(forms[i].handle);return;}
}
