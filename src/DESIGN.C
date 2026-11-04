/* Pixel-based GEM form designer with a persistent, non-executing model. */
#include <aes.h>
#include <vdi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "DESIGN.H"
#include "LANG.H"
#include "MENU.H"
#define KIND (NAME|CLOSER|MOVER)
static FormModel work,initial,previous,before;
static int window,device,wx,wy,ww,wh,vx,vy,vw,vh,px,pw,cx,cy;
static int selected,tool,panx,pany,dirty,undo_ready,event_slot,clip[4];
static int smaller(int a,int b){return a<b?a:b;}
static int larger(int a,int b){return a>b?a:b;}
static int snap(int value){return (value+4)/8*8;}
static void fill(int x,int y,int w,int h,int color)
{
    int p[4];if(w<=0 || h<=0)return;p[0]=x;p[1]=y;p[2]=x+w-1;p[3]=y+h-1;
    vsf_interior(device,1);vsf_color(device,color);vsf_perimeter(device,0);v_bar(device,p);
}
static void border(int x,int y,int w,int h,int color)
{
    fill(x,y,w,1,color);fill(x,y+h-1,w,1,color);fill(x,y,1,h,color);fill(x+w-1,y,1,h,color);
}
static void text(int x,int y,int color,char *value,int width)
{
    char buffer[100];int n;n=smaller(strlen(value),smaller(99,width/8));if(n<=0)return;
    memcpy(buffer,value,n);buffer[n]=0;vst_color(device,color);vswr_mode(device,2);v_gtext(device,x,y,buffer);vswr_mode(device,1);
}
static void button(int x,int y,int width,char *label,int active)
{
    fill(x,y,width,24,active?4:0);border(x,y,width,24,1);text(x+6,y+17,active?0:1,label,width-12);
}
static void layout(void)
{
    wind_get(window,WF_WORKXYWH,&wx,&wy,&ww,&wh);vx=wx+108;vy=wy+40;px=wx+ww-166;pw=160;vw=px-vx-6;vh=wh-68;
    panx=smaller(panx,larger(0,work.width+16-vw));pany=smaller(pany,larger(0,work.height+38-vh));
    cx=vx+8-panx;cy=vy+30-pany;
}
static void error_dialog(void)
{
    char message[128];sprintf(message,tr(T_F_ERROR),form_last_error());form_alert(1,message);
}
static void changed(void){dirty=memcmp(&work,&initial,sizeof(work))!=0;}
static void remember(void){previous=before;undo_ready=1;changed();}
static void undo(void)
{
    if(!undo_ready)return;before=work;work=previous;previous=before;if(selected>=work.count)selected=-1;changed();
}
static void property_value(int property,char *buffer)
{
    FormControl *c;c=selected<0?0:&work.control[selected];buffer[0]=0;
    if(property==FORM_NAME_PROP)strcpy(buffer,c?c->name:work.name);
    else if(property==FORM_TEXT_PROP)strcpy(buffer,c?c->text:work.text);
    else if(property==FORM_X && c)sprintf(buffer,"%d",c->x);
    else if(property==FORM_Y && c)sprintf(buffer,"%d",c->y);
    else if(property==FORM_WIDTH)sprintf(buffer,"%d",c?c->width:work.width);
    else if(property==FORM_HEIGHT)sprintf(buffer,"%d",c?c->height:work.height);
    else if(property==FORM_HANDLER)strcpy(buffer,c?c->handler:work.load);
    else if(property==FORM_CLOSED && !c)strcpy(buffer,work.closed);
}
static void field(int y,int property,int x,int width)
{
    char buffer[64];property_value(property,buffer);fill(x,y,width,22,0);border(x,y,width,22,9);text(x+4,y+16,1,buffer,width-8);
}
static void check(int y,int label,int value)
{
    char buffer[64];sprintf(buffer,"[%c] %s",value?'x':' ',tr(label));text(px+6,y,1,buffer,pw-12);
}
static void properties(void)
{
    FormControl *c;c=selected<0?0:&work.control[selected];fill(px,wy+40,pw,wh-68,8);
    fill(px,wy+40,pw,24,0);text(px+4,wy+57,1,tr(T_F_PROPS),pw-8);
    text(px+6,wy+80,1,tr(T_F_NAME),pw-12);field(wy+84,FORM_NAME_PROP,px+6,pw-12);
    text(px+6,wy+124,1,tr(T_F_TEXT),pw-12);field(wy+128,FORM_TEXT_PROP,px+6,pw-12);
    text(px+6,wy+168,9,tr(T_F_POSITION),pw-12);field(wy+172,FORM_X,px+6,70);field(wy+172,FORM_Y,px+82,72);
    text(px+6,wy+212,1,tr(T_F_SIZE),pw-12);field(wy+216,FORM_WIDTH,px+6,70);field(wy+216,FORM_HEIGHT,px+82,72);
    text(px+6,wy+256,1,c?form_event(c->kind):"Load",pw-12);field(wy+260,FORM_HANDLER,px+6,pw-12);
    if(!c){text(px+6,wy+300,1,tr(T_F_CLOSED),pw-12);field(wy+304,FORM_CLOSED,px+6,pw-12);}
    else{
        check(wy+306,T_F_ENABLED,c->enabled);check(wy+338,T_F_VISIBLE,c->visible);
        if(c->kind==GUI_CHECKBOX)check(wy+370,T_F_CHECKED,c->checked);
    }
}
static void canvas(void)
{
    int region[4],points[256],n,x,y,i,xx,yy,w,h,color;FormControl *c;
    region[0]=larger(clip[0],vx);region[1]=larger(clip[1],vy);region[2]=smaller(clip[2],vx+vw-1);region[3]=smaller(clip[3],vy+vh-1);
    if(region[0]>region[2] || region[1]>region[3])return;
    vs_clip(device,1,region);fill(vx,vy,vw,vh,9);
    fill(cx-2,cy-22,work.width+4,work.height+24,0);border(cx-2,cy-22,work.width+4,work.height+24,selected<0?4:1);
    fill(cx-1,cy-21,work.width+2,20,8);text(cx+4,cy-6,1,work.text,work.width-8);fill(cx,cy,work.width,work.height,0);
    vsm_type(device,1);vsm_color(device,9);n=0;
    for(y=0;y<work.height;y+=8)for(x=0;x<work.width;x+=8){
        xx=cx+x;yy=cy+y;if(xx<vx || xx>=vx+vw || yy<vy || yy>=vy+vh)continue;
        points[n*2]=xx;points[n*2+1]=yy;if(++n==128){v_pmarker(device,n,points);n=0;}
    }
    if(n)v_pmarker(device,n,points);
    for(i=0;i<work.count;++i){
        c=&work.control[i];x=cx+c->x;y=cy+c->y;w=c->width;h=c->height;color=c->enabled && c->visible?1:9;
        fill(x,y,w,h,c->visible?0:8);
        if(c->kind==GUI_BUTTON || c->kind==GUI_TEXTBOX)border(x,y,w,h,color);
        if(c->kind==GUI_CHECKBOX){border(x+2,y+(h-12)/2,12,12,color);if(c->checked)text(x+3,y+(h+10)/2,color,"x",12);text(x+20,y+(h+12)/2,color,c->text,w-24);}
        else text(x+4,y+(h+12)/2,color,c->text,w-8);
        if(i==selected){border(x-1,y-1,w+2,h+2,4);fill(x+w-5,y+h-5,6,6,4);fill(x-3,y-3,5,5,4);}
    }
    if(selected<0)fill(cx+work.width-5,cy+work.height-5,6,6,4);
    vs_clip(device,1,clip);
}
static void paint(void)
{
    char buffer[100];int i,y;layout();fill(wx,wy,ww,wh,8);
    button(wx+6,wy+6,90,tr(T_F_APPLY),0);button(wx+102,wy+6,90,tr(T_CANCEL),0);
    button(wx+198,wy+6,90,tr(T_F_RUN),1);button(wx+294,wy+6,90,tr(T_F_UNDO),0);button(wx+390,wy+6,104,tr(T_F_CODE),0);
    text(wx+504,wy+23,1,"F1 Help",ww-510);
    fill(wx+6,wy+40,96,wh-68,8);text(wx+9,wy+57,1,tr(T_F_TOOLS),90);
    for(i=0;i<5;++i){y=wy+72+i*34;button(wx+8,y,92,i?form_kind(i+1):tr(T_F_SELECT),tool==(i?i+1:0));}
    button(wx+8,wy+258,92,tr(T_F_DELETE),0);
    button(wx+8,wy+298,42,"<",0);button(wx+56,wy+298,42,">",0);button(wx+8,wy+330,42,"^",0);button(wx+56,wy+330,42,"v",0);
    canvas();properties();fill(wx,wy+wh-22,ww,22,4);
    sprintf(buffer,"%.23s%s | %s",selected<0?work.name:work.control[selected].name,dirty?" *":"",tr(T_F_HINT));text(wx+6,wy+wh-6,0,buffer,ww-12);
}
static void redraw(int x,int y,int w,int h)
{
    int rx,ry,rw,rh;wind_update(BEG_UPDATE);graf_mouse(M_OFF,0L);
    wind_get(window,WF_FIRSTXYWH,&rx,&ry,&rw,&rh);
    while(rw && rh){
        clip[0]=larger(rx,x);clip[1]=larger(ry,y);clip[2]=smaller(rx+rw,x+w)-1;clip[3]=smaller(ry+rh,y+h)-1;
        if(clip[0]<=clip[2] && clip[1]<=clip[3]){vs_clip(device,1,clip);paint();}
        wind_get(window,WF_NEXTXYWH,&rx,&ry,&rw,&rh);
    }
    vs_clip(device,0,clip);graf_mouse(M_ON,0L);wind_update(END_UPDATE);
}
static void refresh(void){layout();redraw(wx,wy,ww,wh);}
static void edit_property(int property)
{
    char value[64],prompt[64];int label,capacity;
    if(property==FORM_HANDLER && selected>=0 && work.control[selected].kind==GUI_LABEL){form_alert(1,tr(T_F_NOEVENT));return;}
    label=property==FORM_NAME_PROP?T_F_NAME:property==FORM_TEXT_PROP?T_F_TEXT:property==FORM_WIDTH||property==FORM_HEIGHT?T_F_SIZE:property==FORM_HANDLER?T_F_EVENT:property==FORM_CLOSED?T_F_CLOSED:T_F_POSITION;
    strcpy(prompt,tr(label));property_value(property,value);capacity=property==FORM_TEXT_PROP?64:property==FORM_NAME_PROP||property==FORM_HANDLER||property==FORM_CLOSED?24:8;
    if(!input_dialog(prompt,value,capacity))return;before=work;
    if(!form_set(&work,selected,property,value)){error_dialog();return;}remember();
    if(property==FORM_HANDLER || property==FORM_CLOSED)event_slot=property==FORM_CLOSED;
}
static int code_handler(char *handler)
{
    char value[FORM_NAME],*name,*event;int n,property;
    if(selected>=0 && work.control[selected].kind==GUI_LABEL){form_alert(1,tr(T_F_NOEVENT));return 0;}
    property=selected<0 && event_slot?FORM_CLOSED:FORM_HANDLER;property_value(property,value);
    if(!value[0]){
        name=selected<0?work.name:work.control[selected].name;event=selected<0 && event_slot?"Closed":form_event(selected<0?GUI_WINDOW:work.control[selected].kind);
        n=23-strlen(event)-1;strncpy(value,name,n);value[n]=0;strcat(value,"_");strcat(value,event);before=work;
        if(!form_set(&work,selected,property,value)){error_dialog();return 0;}remember();
    }
    strcpy(handler,value);return DESIGN_CODE;
}
static int cancel(void)
{
    int choice;if(!dirty)return 0;choice=form_alert(1,tr(T_F_DIRTY));return choice==1?DESIGN_APPLY:choice==2?0:-1;
}
static void move_key(int scan,int state)
{
    FormControl *c;int x,y,w,h,dx,dy;dx=scan==75?-8:scan==77?8:0;dy=scan==72?-8:scan==80?8:0;
    if(state & 4){panx=larger(0,panx+dx);pany=larger(0,pany+dy);return;}
    before=work;
    if(selected<0){
        if(!(state & 3))return;
        {char number[12];sprintf(number,"%d",dx?work.width+dx:work.height+dy);if(!form_set(&work,-1,dx?FORM_WIDTH:FORM_HEIGHT,number)){error_dialog();return;}}
    }else{
        c=&work.control[selected];x=c->x;y=c->y;w=c->width;h=c->height;
        if(state & 3){w+=dx;h+=dy;}else{x+=dx;y+=dy;}
        if(!form_bounds(&work,selected,x,y,w,h)){error_dialog();return;}
    }
    remember();
}
static void drag_control(int mx,int my)
{
    FormControl *c;int x,y,w,h,nx,ny,bx,by,bw,bh,resize;
    c=&work.control[selected];x=cx+c->x;y=cy+c->y;w=c->width;h=c->height;before=work;
    resize=mx>=x+w-7 && my>=y+h-7;
    if(resize){
        bx=x+16-6;by=y+16-6;bw=smaller(cx+work.width,vx+vw)-bx;bh=smaller(cy+work.height,vy+vh)-by;
        if(bw<6 || bh<6)return;
        graf_dragbox(6,6,x+w-6,y+h-6,bx,by,bw,bh,&nx,&ny);
        if(nx==x+w-6 && ny==y+h-6)return;
        w=smaller(work.width-c->x,larger(16,snap(nx+6-x)));h=smaller(work.height-c->y,larger(16,snap(ny+6-y)));
        if(!form_bounds(&work,selected,c->x,c->y,w,h)){error_dialog();return;}
    }else{
        bx=larger(cx,vx);by=larger(cy,vy);bw=smaller(cx+work.width,vx+vw)-bx;bh=smaller(cy+work.height,vy+vh)-by;
        if(w>bw || h>bh)return;
        graf_dragbox(w,h,x,y,bx,by,bw,bh,&nx,&ny);
        if(nx==x && ny==y)return;
        nx=smaller(work.width-w,larger(0,snap(nx-cx)));ny=smaller(work.height-h,larger(0,snap(ny-cy)));
        if(!form_bounds(&work,selected,nx,ny,w,h)){error_dialog();return;}
    }
    if(memcmp(&work,&before,sizeof(work)))remember();
}
static void resize_form(void)
{
    int i,minw,minh,bx,by,bw,bh,x,y,w,h;char value[12];
    minw=120;minh=64;
    for(i=0;i<work.count;++i){minw=larger(minw,work.control[i].x+work.control[i].width);minh=larger(minh,work.control[i].y+work.control[i].height);}
    bx=cx+minw-6;by=cy+minh-6;bw=smaller(cx+600,vx+vw)-bx;bh=smaller(cy+400,vy+vh)-by;
    if(bw<6 || bh<6)return;
    graf_dragbox(6,6,cx+work.width-6,cy+work.height-6,bx,by,bw,bh,&x,&y);
    if(x==cx+work.width-6 && y==cy+work.height-6)return;
    w=larger(minw,smaller(600,snap(x+6-cx)));h=larger(minh,smaller(400,snap(y+6-cy)));before=work;
    sprintf(value,"%d",w);if(!form_set(&work,-1,FORM_WIDTH,value)){error_dialog();return;}
    sprintf(value,"%d",h);if(!form_set(&work,-1,FORM_HEIGHT,value)){work=before;error_dialog();return;}
    if(memcmp(&work,&before,sizeof(work)))remember();
}
static int mouse(int mx,int my)
{
    int i,property,x,y;
    if(my>=wy+6 && my<wy+30){
        if(mx<wx+96)return DESIGN_APPLY;
        if(mx<wx+192)return cancel();
        if(mx<wx+288)return DESIGN_RUN;
        if(mx<wx+384)undo();else if(mx<wx+494)return DESIGN_SOURCE;
        return -1;
    }
    if(mx>=wx+6 && mx<wx+102){
        for(i=0;i<5;++i)if(my>=wy+72+i*34 && my<wy+96+i*34){tool=i?i+1:0;return -1;}
        if(my>=wy+258 && my<wy+282 && selected>=0){before=work;form_delete(&work,selected);selected=-1;remember();}
        if(my>=wy+298 && my<wy+322)panx=larger(0,panx+(mx<wx+54?-32:32));
        if(my>=wy+330 && my<wy+354)pany=larger(0,pany+(mx<wx+54?-32:32));return -1;
    }
    if(mx>=px && mx<px+pw){
        property=-1;
        if(my>=wy+84 && my<wy+106)property=FORM_NAME_PROP;
        else if(my>=wy+128 && my<wy+150)property=FORM_TEXT_PROP;
        else if(my>=wy+172 && my<wy+194 && selected>=0)property=mx<px+80?FORM_X:FORM_Y;
        else if(my>=wy+216 && my<wy+238)property=mx<px+80?FORM_WIDTH:FORM_HEIGHT;
        else if(my>=wy+260 && my<wy+282)property=FORM_HANDLER;
        else if(selected<0 && my>=wy+304 && my<wy+326)property=FORM_CLOSED;
        else if(selected>=0){
            if(my>=wy+290 && my<wy+314)property=FORM_ENABLED;
            else if(my>=wy+322 && my<wy+346)property=FORM_VISIBLE;
            else if(my>=wy+354 && my<wy+378 && work.control[selected].kind==GUI_CHECKBOX)property=FORM_CHECKED;
            if(property>=FORM_ENABLED){before=work;i=property==FORM_ENABLED?work.control[selected].enabled:property==FORM_VISIBLE?work.control[selected].visible:work.control[selected].checked;
                if(form_set(&work,selected,property,i?"0":"1"))remember();return -1;}
        }
        if(property>=0)edit_property(property);return -1;
    }
    if(mx<vx || mx>=vx+vw || my<vy || my>=vy+vh)return -1;x=mx-cx;y=my-cy;
    if(tool && x>=0 && y>=0 && x<work.width && y<work.height){
        before=work;if(!form_add(&work,tool,snap(x),snap(y)))error_dialog();else{selected=work.count-1;tool=0;event_slot=0;remember();}return -1;
    }
    selected=-1;
    for(i=work.count-1;i>=0;--i)if(x>=work.control[i].x && x<work.control[i].x+work.control[i].width && y>=work.control[i].y && y<work.control[i].y+work.control[i].height){selected=i;break;}
    event_slot=0;if(selected>=0)drag_control(mx,my);
    else if(x>=work.width-7 && x<work.width && y>=work.height-7 && y<work.height)resize_form();return -1;
}
int designer_edit(FormModel *model,int vdi,int parent,char *filename,DesignHost host,char *handler)
{
    int dx,dy,dw,dh,e,m[8],mx,my,mb,ks,key,clicks,scan,result;char title[100];
    work=initial=*model;device=vdi;selected=-1;tool=panx=pany=dirty=undo_ready=event_slot=0;handler[0]=0;
    wind_get(0,WF_WORKXYWH,&dx,&dy,&dw,&dh);window=wind_create(KIND,dx,dy,dw,dh);if(window<0)return 0;
    sprintf(title,"TT BASIC 0.10 - %s - %.32s",tr(T_F_TITLE),filename);wind_set(window,WF_NAME,title);wind_open(window,dx,dy,dw,dh);
    menus_modal(1);layout();refresh();result=-1;
    while(result<0){
        e=evnt_multi(MU_MESAG|MU_KEYBD|MU_BUTTON,1,1,1,0,0,0,0,0,0,0,0,0,0,m,0,0,&mx,&my,&mb,&ks,&key,&clicks);
        if(e & MU_MESAG){
            if(m[0]==MN_SELECTED)menus_event(m[3],m[4]);
            else if(m[3]==window){
                if(m[0]==WM_REDRAW)redraw(m[4],m[5],m[6],m[7]);
                else if(m[0]==WM_CLOSED)result=cancel();
                else if(m[0]==WM_TOPPED)wind_set(window,WF_TOP,0,0,0,0);
                else if(m[0]==WM_MOVED){wind_set(window,WF_CURRXYWH,m[4],m[5],m[6],m[7]);refresh();}
            }else if(host)host(m);
        }
        if(e & MU_KEYBD){
            scan=(key>>8)&255;if(scan>=84 && scan<=93)scan-=25;
            if((ks & 4) && scan==32)result=DESIGN_SOURCE;
            else if((key & 255)==27)result=cancel();
            else if(scan==59)form_alert(1,tr(T_F_HELP));
            else if(scan==60)result=DESIGN_SAVE;
            else if(scan==62)result=code_handler(handler)?DESIGN_CODE:-1;
            else if(scan==63)result=DESIGN_RUN;
            else if((ks & 4) && scan==31)result=DESIGN_SAVE;
            else if((ks & 4) && scan==44)undo();
            else if(scan==83 && selected>=0){before=work;form_delete(&work,selected);selected=-1;remember();}
            else if(scan==72 || scan==80 || scan==75 || scan==77)move_key(scan,ks);
            refresh();
        }
        if(result<0 && (e & MU_BUTTON) && wind_find(mx,my)==window){result=mouse(mx,my);refresh();evnt_button(1,1,0,&mx,&my,&mb,&ks);}
    }
    if(result)*model=work;
    wind_close(window);wind_delete(window);menus_modal(0);wind_set(parent,WF_TOP,0,0,0,0);return result;
}
