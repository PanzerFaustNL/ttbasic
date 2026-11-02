#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BASIC.H"
static char source[MAX_LINES][LINE_LEN],output[OUT_LINES][OUT_LEN];
static int used,resets,created,event_index,events[16][2],event_count,shown,closed,checks;
static BasicGuiRequest nodes[GUI_LIMIT];
static int mock_gui(BasicGuiRequest *r)
{
    int id;id=r->id;
    if(r->operation==GUI_RESET){++resets;return 1;}
    if(r->operation==GUI_CREATE){nodes[id]=*r;++created;return 1;}
    if(r->operation==GUI_SHOW){++shown;return 1;}
    if(r->operation==GUI_CLOSE){++closed;return 1;}
    if(r->operation==GUI_NEXT){if(event_index==event_count)return 0;r->id=events[event_index][0];r->event=events[event_index++][1];return 1;}
    if(r->operation==GUI_SET){if(!strcmp(r->property,"TEXT"))strcpy(nodes[id].text,r->text);else nodes[id].number=r->number;return 1;}
    if(r->operation==GUI_GET){strcpy(r->text,nodes[id].text);r->number=nodes[id].number;return 1;}
    return -1;
}
static int run(char *text)
{
    int n,k;char *p;n=k=0;memset(source,0,sizeof(source));
    for(p=text;*p;++p){if(*p=='\n'){++n;k=0;}else {assert(k<LINE_LEN-1);source[n][k++]=*p;}}
    event_index=0;created=shown=closed=0;resets=0;++checks;return basic_run(source,n+1,output,&used);
}
static int has(char *text)
{
    int i;for(i=0;i<used;++i)if(strstr(output[i],text))return 1;return 0;
}
static int debug_count,seen[100],depths[100],reasons[100],saw_local,saw_field,saw_global,saw_error,stop_at,error_depth;
static int debugger(char *path,int row,int depth,int reason)
{
    int i,n,line;char frame[48],where[BASIC_PATH];BasicDebugValue value;
    assert(strstr(path,"MAIN.BAS"));assert(basic_debug_frames()==depth+1);
    assert(basic_debug_frame(depth,frame,where,&line));assert(line==row);
    if(debug_count<100){seen[debug_count]=row;depths[debug_count]=depth;reasons[debug_count++]=reason;}
    n=basic_debug_count(depth);
    for(i=0;i<n;++i){assert(basic_debug_variable(depth,i,&value));
        if(!strcmp(value.name,"LOCALVALUE") && !strcmp(value.value,"8"))saw_local=1;
        if(!strcmp(value.name,"Me.VALUE") && !strcmp(value.value,"42"))saw_field=1;
        if(!strcmp(value.name,"GLOBALVALUE") && !strcmp(value.value,"7"))saw_global=1;
    }
    assert(!basic_debug_variable(depth,n,&value));
    if(reason==BASIC_DEBUG_ERROR){assert(strlen(basic_debug_error())>0);saw_error=1;error_depth=depth;}
    return row!=stop_at;
}
int main(int argc,char **argv)
{
    FILE *file;char path[BASIC_PATH];int n,len;
    basic_set_gui(mock_gui);
    events[0][0]=1;events[0][1]=GUI_CLICK;event_count=1;
    assert(!run("Dim form As New Window(\"Hello\", 320, 180)\nDim buttonA As New Button(form, \"OK\", 10, 10, 80, 24)\nDim textA As New Label(form, \"Before\", 10, 50, 180, 24)\nAddHandler buttonA.Click, AddressOf HandleClick\nform.Show()\nApplication.Run()\nPrint textA.Text\nEnd\nSub HandleClick(sender As Button)\ntextA.Text = sender.Text & \" clicked\"\nform.Close()\nEnd Sub"));
    assert(created==3 && shown==1 && closed==1 && resets==2 && has("OK clicked"));
    assert(!run("Dim form As New Window(\"Hello\",320,180)\nDim b As New Button(form,\"OK\",10,10,80,24)\nDim controller As New Controller\nAddHandler b.Click, AddressOf controller.Clicked\nAddHandler b.Click, AddressOf controller.Clicked\nApplication.Run()\nPrint controller.Count\nEnd\nClass Controller\nPublic Count As Integer\nPublic Sub Clicked()\nMe.Count = Me.Count + 1\nEnd Sub\nEnd Class"));assert(has("1"));
    assert(!run("Dim form As New Window(\"Hello\",320,180)\nDim b As New Button(form,\"OK\",10,10,80,24)\nAddHandler b.Click, AddressOf Clicked\nRemoveHandler b.Click, AddressOf Clicked\nApplication.Run()\nPrint \"Done\"\nEnd\nSub Clicked()\nPrint \"UNWANTED\"\nEnd Sub"));assert(!has("UNWANTED") && has("Done"));
    assert(!run("Dim form As New Window(\"Hello\",320,180)\nDim b As New Button(form,\"OK\",10,10,80,24)\nAddHandler b.Click, AddressOf Clicked\nApplication.Run()\nEnd\nSub Clicked()\nApplication.Exit()\nEnd Sub"));
    assert(run("Dim form As New Window(\"Bad\",65537,180)"));assert(!created);
    assert(run("Dim form As New Window(\"Bad\",320,180)\nDim b As New Button(form,\"OK\",65537,0,80,24)"));assert(created==1);
    assert(run("Dim form As New Window(\"X\",320,180)\nDim b As New Button(form,\"OK\",10,10,80,24)\nAddHandler b.Change, AddressOf Clicked\nEnd\nSub Clicked()\nEnd Sub"));assert(has("Event is not supported"));
    assert(run("Dim form As New Window(\"X\",320,180)\nDim b As New Button(form,\"OK\",10,10,80,24)\nAddHandler b.Click, AddressOf Clicked\nEnd\nSub Clicked(sender As Integer)\nEnd Sub"));assert(has("requires number"));
    assert(run("Dim form As New Window(\"X\",320,180)\nDim b As Button\nb = form"));assert(has("type does not match"));
    assert(!run("Dim form As Window\nform = Nothing\nPrint 7"));assert(has("7"));
    assert(!run("Dim form As New Window(\"X\",320,180)\nDim box As New TextBox(form,\"hello\",10,10,180,24)\nDim labelA As New Label(form,\"world\",10,50,180,24)\nbox.Text = box.Text & labelA.Text\nPrint box.Text"));assert(has("helloworld"));
    assert(!run("Dim form As New Window(\"X\",320,180)\nDim alias As Object\nalias = form\nPrint alias Is form\nPrint form IsNot Nothing\nPrint TypeOf form Is Window\nPrint TypeOf alias Is Object"));assert(!strcmp(output[0],"-1") && !strcmp(output[1],"-1") && !strcmp(output[2],"-1") && !strcmp(output[3],"-1"));
    basic_set_gui(0);assert(run("Dim form As New Window(\"X\",320,180)"));assert(has("not available"));
    basic_set_debug(debugger);debug_count=0;stop_at=-1;
    assert(!run("Dim globalValue As Integer = 7\nDim obj As New Demo\nobj.Go(8)\nEnd\nClass Demo\nPublic value As Integer = 42\nPublic Sub Go(localValue As Integer)\nPrint localValue\nEnd Sub\nEnd Class"));
    assert(saw_local && saw_field && saw_global);assert(debug_count>=4 && depths[2]==0);
    debug_count=0;assert(!run("Dim i As Integer\nFor i = 1 To 2\nPrint i\nNext\nEnd"));
    assert(seen[0]==1 && seen[1]==2 && seen[2]==3 && seen[3]==4 && seen[4]==2);
    debug_count=0;assert(!run("Dim i As Integer = 0\nWhile i < 2\ni = i + 1\nWend\nEnd"));assert(seen[4]==2);
    debug_count=0;assert(!run("Stop\nPrint 42"));assert(reasons[1]==BASIC_DEBUG_STOP && has("42"));
    debug_count=0;stop_at=2;assert(!run("Print 1\nPrint 2\nPrint 3"));assert(has("1") && !has("2") && !has("3"));
    stop_at=-1;saw_error=0;assert(run("Dim n As Integer = 3\nPrint n / 0"));assert(saw_error);
    saw_error=0;assert(run("Crash(9)\nEnd\nSub Crash(secret As Integer)\nPrint secret / 0\nEnd Sub"));assert(saw_error && error_depth==1);
    basic_set_debug(0);assert(basic_debug_frames()==0);assert(!basic_debug_count(0));
    if(argc>1){
        assert(strlen(argv[1])+14<BASIC_PATH);sprintf(path,"%s/GEMDEMO.BAS",argv[1]);file=fopen(path,"r");assert(file);n=0;
        while(n<MAX_LINES && fgets(source[n],LINE_LEN,file)){len=strlen(source[n]);while(len && (source[n][len-1]=='\n'||source[n][len-1]=='\r'))source[n][--len]=0;++n;}fclose(file);
        basic_set_gui(mock_gui);event_index=0;event_count=4;events[0][0]=0;events[0][1]=GUI_LOAD;
        events[1][0]=4;events[1][1]=GUI_CLICK;events[2][0]=5;events[2][1]=GUI_CLICK;events[3][0]=0;events[3][1]=GUI_CLOSED;
        assert(!basic_run_project(source,n,path,output,&used));assert(has("Click 1: Hello Marco") && has("Window closed.") && has("GEM demo finished."));
        basic_set_gui(0);++checks;
    }
    printf("%d GUI/event/debugger checks passed.\n",checks);return 0;
}
