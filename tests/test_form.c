#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "FORM.H"
static FormModel model,roundtrip,saved;
static char source[MAX_LINES][LINE_LEN],output[MAX_LINES][LINE_LEN],again[MAX_LINES][LINE_LEN];
static char console[OUT_LINES][OUT_LEN];static int count,used,checks;
static BasicGuiRequest nodes[GUI_LIMIT];
static int sample_events,event_index;
static int gui(BasicGuiRequest *r)
{
    if(r->operation==GUI_CREATE){nodes[r->id]=*r;return 1;}
    if(r->operation==GUI_SET){if(!strcmp(r->property,"TEXT"))strcpy(nodes[r->id].text,r->text);else nodes[r->id].number=r->number;return 1;}
    if(r->operation==GUI_GET){strcpy(r->text,nodes[r->id].text);r->number=nodes[r->id].number;return 1;}
    if(r->operation==GUI_NEXT){if(sample_events && event_index<3){r->id=event_index==0?4:event_index==1?5:0;r->event=event_index==2?GUI_CLOSED:GUI_CLICK;++event_index;return 1;}return 0;}return 1;
}
static void ok(int result){++checks;if(!result)fprintf(stderr,"%s\n",form_last_error());assert(result);}
int main(int argc,char **argv)
{
    int i,n,line,len;char text[64],path[BASIC_PATH];FILE *file;
    form_default(&model);ok(form_validate(&model));ok(form_add(&model,GUI_BUTTON,16,16));
    ok(form_add(&model,GUI_TEXTBOX,16,64));ok(form_add(&model,GUI_CHECKBOX,16,104));
    ok(form_set(&model,0,FORM_TEXT_PROP,"Say \"Hello\""));ok(form_set(&model,2,FORM_CHECKED,"1"));
    ok(form_generate(&model,source,0,output,&count));ok(form_read(output,count,&roundtrip)==1);assert(!memcmp(&model,&roundtrip,sizeof(model)));
    ok(form_generate(&roundtrip,output,count,again,&n));assert(n==count);for(i=0;i<n;++i)assert(!strcmp(output[i],again[i]));
    basic_set_gui(gui);assert(!basic_run(output,count,console,&used));basic_set_gui(0);
    line=form_handler_line(output,count,model.control[0].handler);assert(line>=0);
    strcpy(output[line+1],"    Print \"User code survives\"");
    ok(form_set(&model,0,FORM_NAME_PROP,"GreetButton"));ok(form_set(&model,0,FORM_X,"24"));
    ok(form_generate(&model,output,count,again,&n));line=form_handler_line(again,n,model.control[0].handler);assert(!strcmp(again[line+1],"    Print \"User code survives\""));
    saved=model;assert(!form_set(&model,0,FORM_NAME_PROP,model.name));assert(!memcmp(&model,&saved,sizeof(model)));
    assert(!form_set(&model,0,FORM_NAME_PROP,"If"));assert(!form_set(&model,0,FORM_X,"65537"));assert(!form_set(&model,-1,FORM_WIDTH,"130"));assert(!memcmp(&model,&saved,sizeof(model)));
    ok(form_delete(&model,0));ok(form_generate(&model,again,n,output,&count));assert(form_handler_line(output,count,"Button1_Click")>=0);
    ok(form_add(&model,GUI_BUTTON,8,8));assert(strcmp(model.control[model.count-1].name,"Button1"));
    strcpy(source[0],"Print 42");assert(!form_generate(&model,source,1,output,&n));assert(form_read(source,1,&roundtrip)==0);
    strcpy(source[0],"Rem @TTFORM BEGIN 1");assert(form_read(source,1,&roundtrip)==-1);
    strcpy(source[1],"Rem @TTFORM END");assert(form_read(source,2,&roundtrip)==-1);
    form_default(&model);memset(text,'"',63);text[63]=0;ok(form_set(&model,-1,FORM_TEXT_PROP,text));
    ok(form_add(&model,GUI_BUTTON,8,8));ok(form_set(&model,0,FORM_TEXT_PROP,text));ok(form_generate(&model,source,0,output,&count));ok(form_read(output,count,&roundtrip)==1);assert(!strcmp(roundtrip.text,text));
    basic_set_gui(gui);assert(!basic_run(output,count,console,&used));basic_set_gui(0);
    for(i=0;i<count;++i)assert(strlen(output[i])<LINE_LEN);
    strcpy(output[3],"Rem @TTFORM T 0 \"broken\"\"");assert(form_read(output,count,&roundtrip)==-1);
    form_default(&model);ok(form_add(&model,GUI_BUTTON,8,8));ok(form_generate(&model,source,0,output,&count));
    strcpy(output[count++],"Function Button1_Click() As Integer");strcpy(output[count++],"Return 0");strcpy(output[count++],"End Function");
    line=form_handler_line(output,count,"Button1_Click");assert(line>=0);
    strcpy(output[line],"Sub Other(sender As Button)");assert(!form_generate(&model,output,count,again,&n));
    form_default(&model);for(i=0;i<FORM_CONTROLS;++i)ok(form_add(&model,GUI_BUTTON,8,8));saved=model;assert(!form_add(&model,GUI_BUTTON,8,8));assert(!memcmp(&model,&saved,sizeof(model)));
    ok(form_generate(&model,source,0,output,&count));assert(count<MAX_LINES);
    for(i=count;i<MAX_LINES;++i)strcpy(output[i],"Rem user notes");ok(form_set(&model,0,FORM_HANDLER,"NewClick"));assert(!form_generate(&model,output,MAX_LINES,again,&n));
    if(argc>1){
        assert(strlen(argv[1])+13<BASIC_PATH);sprintf(path,"%s/DESIGN.BAS",argv[1]);file=fopen(path,"r");assert(file);count=0;
        while(count<MAX_LINES && fgets(source[count],LINE_LEN,file)){len=strlen(source[count]);while(len && (source[count][len-1]=='\r' || source[count][len-1]=='\n'))source[count][--len]=0;++count;}fclose(file);
        ok(form_read(source,count,&model)==1);ok(form_generate(&model,source,count,output,&n));assert(n==count);for(i=0;i<count;++i)assert(!strcmp(source[i],output[i]));
        sample_events=1;event_index=0;basic_set_gui(gui);assert(!basic_run_project(output,n,path,console,&used));assert(!strcmp(nodes[6].text,"Hello Marco"));basic_set_gui(0);++checks;
    }
    printf("%d form model/generation checks passed.\n",checks);return 0;
}
