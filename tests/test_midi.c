#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "BASIC.H"
#include "MIDI.H"
static char source[MAX_LINES][LINE_LEN],out[OUT_LINES][OUT_LEN];
static unsigned char packets[2048][MIDI_PACKET];static int sizes[2048],packet_count,used,checks;
static int writer(unsigned char *bytes,int count)
{
    assert(packet_count<2048);memcpy(packets[packet_count],bytes,count);sizes[packet_count++]=count;return 1;
}
static int failing_writer(unsigned char *bytes,int count){(void)bytes;(void)count;return 0;}
static void packet(int index,int a,int b,int c,int size)
{
    assert(index<packet_count && sizes[index]==size);assert(packets[index][0]==a && packets[index][1]==b);if(size>2)assert(packets[index][2]==c);++checks;
}
static int run(char *text,char *path)
{
    int n,k,result;char *p;n=k=0;memset(source,0,sizeof(source));
    for(p=text;*p;++p){if(*p=='\n'){++n;k=0;}else {assert(k<LINE_LEN-1);source[n][k++]=*p;}}
    packet_count=0;++checks;result=basic_run_project(source,n+1,path,out,&used);return result;
}
static int has(char *text){int i;for(i=0;i<used;++i)if(strstr(out[i],text))return 1;return 0;}
static BasicGuiRequest controls[GUI_LIMIT];
static int events[16][2],event_count,event_index,invalid_input,arturia_test;
static int gui(BasicGuiRequest *r)
{
    if(r->operation==GUI_CREATE){controls[r->id]=*r;controls[r->id].number=0;return 1;}
    if(r->operation==GUI_SET){if(!strcmp(r->property,"TEXT"))strcpy(controls[r->id].text,r->text);else controls[r->id].number=r->number;return 1;}
    if(r->operation==GUI_GET){strcpy(r->text,controls[r->id].text);r->number=controls[r->id].number;return 1;}
    if(r->operation==GUI_NEXT){
        if(event_index==event_count)return 0;r->id=events[event_index][0];r->event=events[event_index++][1];
        if(r->id==5)controls[5].number=-1;
        if(r->id==6)controls[6].number=0;
        if(arturia_test && r->id==11)strcpy(controls[7].text,arturia_test==1?"32":"1");
        if(invalid_input && r->id==11)strcpy(controls[7].text,"33");return 1;
    }return 1;
}
static int demo(char *directory,int live,int invalid)
{
    FILE *file;char path[BASIC_PATH];int n,len,result;
    sprintf(path,strchr(directory,'\\')?"%s\\DX7DEMO.BAS":"%s/DX7DEMO.BAS",directory);file=fopen(path,"r");assert(file);n=0;
    while(n<MAX_LINES && fgets(source[n],LINE_LEN,file)){len=strlen(source[n]);while(len && (source[n][len-1]=='\r'||source[n][len-1]=='\n'))source[n][--len]=0;++n;}fclose(file);
    event_count=0;event_index=0;invalid_input=invalid;packet_count=0;
    events[event_count][0]=0;events[event_count++][1]=GUI_LOAD;
    if(!arturia_test){events[event_count][0]=6;events[event_count++][1]=GUI_CLICK;}
    if(live){events[event_count][0]=5;events[event_count++][1]=GUI_CLICK;}
    events[event_count][0]=12;events[event_count++][1]=GUI_CLICK;
    events[event_count][0]=11;events[event_count++][1]=GUI_CLICK;
    events[event_count][0]=13;events[event_count++][1]=GUI_CLICK;
    if(!arturia_test){events[event_count][0]=4;events[event_count++][1]=GUI_CLICK;}
    events[event_count][0]=16;events[event_count++][1]=GUI_CLICK;
    events[event_count][0]=0;events[event_count++][1]=GUI_CLOSED;
    basic_set_gui(gui);result=basic_run_project(source,n,path,out,&used);basic_set_gui(0);++checks;return result;
}
int main(int argc,char **argv)
{
    unsigned char bytes[]={240,67,16,1,6,5,247};char path[BASIC_PATH];int i;
    assert(argc==2);assert(strlen(argv[1])+10<BASIC_PATH);sprintf(path,strchr(argv[1],'\\')?"%s\\MAIN.BAS":"%s/MAIN.BAS",argv[1]);
    midi_set_writer(writer);midi_set_logger(0);midi_begin();packet_count=0;
    assert(midi_note_on(1,60,96));assert(!packet_count && strstr(midi_last_message(),"DRY: 90 3C 60"));
    assert(midi_enable(1));assert(!packet_count);assert(midi_enabled());
    assert(midi_note_on(16,127,127));packet(0,159,127,127,3);
    assert(midi_note_off(16,127,0));packet(1,143,127,0,3);
    assert(midi_program(1,31));packet(2,192,31,0,2);
    assert(midi_pitch(2,8192));packet(3,225,0,64,3);
    assert(midi_sysex(bytes,7));assert(!memcmp(packets[4],bytes,7));
    assert(!midi_note_on(65537L,60,96));assert(!midi_note_on(1,65537L,96));assert(!midi_program(0,1));assert(!midi_pitch(1,16384));
    bytes[3]=128;assert(!midi_sysex(bytes,7));bytes[3]=1;
    assert(midi_note_on(1,60,96));assert(midi_control(1,64,127));i=packet_count;
    assert(midi_finish());packet(i,176,64,0,3);packet(i+1,128,60,0,3);assert(!midi_enabled());
    midi_begin();assert(midi_enable(1));packet_count=0;assert(midi_panic(1));assert(packet_count==129);packet(0,176,64,0,3);packet(128,128,127,0,3);assert(midi_finish());
    assert(!run("Midi.NoteOn(1,60,96)\nPrint Midi.LastMessage()",path));assert(!packet_count && has("DRY: 90 3C 60"));
    assert(!run("Midi.EnableOutput(True)\nMidi.NoteOn(1,60,96)\nEnd",path));assert(packet_count==2);packet(1,128,60,0,3);
    assert(run("Midi.EnableOutput(True)\nMidi.NoteOn(1,60,96)\nPrint 1 / 0",path));assert(packet_count==2);packet(1,128,60,0,3);
    assert(run("Midi.NoteOn(1,256,96)",path));assert(!packet_count && has("0..127"));
    assert(run("Midi.SysEx(Array(240,67,128,247))",path));assert(has("7-bit"));
    assert(run("Midi.SysEx(Array(240,67,65537,247))",path));assert(has("0..255"));
    assert(run("Midi.NoteOn(1,60)",path));assert(has("number of MIDI arguments"));
    assert(run("Print Midi.NoteOn(1,60,96)",path));assert(has("no result"));
    assert(run("Print Midi.CheckRange(33,1,32)",path));assert(has("outside requested"));
    midi_set_writer(failing_writer);assert(run("Midi.EnableOutput(True)\nMidi.ProgramChange(1,0)",path));assert(has("MIDI output failed"));
    midi_set_writer(0);assert(run("Midi.EnableOutput(True)",path));assert(has("not available"));
    midi_set_writer(writer);
    assert(!run("Imports \"DX7.BAS\"\nDim synth As New DX7(1)\nMidi.EnableOutput(True)\nsynth.Algorithm(6)\nsynth.Feedback(7)\nsynth.OperatorLevel(1,99)\nsynth.OperatorLevel(6,1)\nEnd",path));
    assert(packet_count==4);assert(packets[0][3]==1 && packets[0][4]==6 && packets[0][5]==5);
    assert(packets[1][3]==1 && packets[1][4]==7 && packets[1][5]==7);
    assert(packets[2][3]==0 && packets[2][4]==121 && packets[2][5]==99);
    assert(packets[3][3]==0 && packets[3][4]==16 && packets[3][5]==1);
    assert(!run("Imports \"DX7.BAS\"\nDim synth As New DX7(16)\nMidi.EnableOutput(True)\nDim i As Integer\nFor i = 1 To 100\nsynth.Algorithm(1)\nNext\nEnd",path));assert(packet_count==100 && packets[0][2]==31);
    assert(run("Imports \"DX7.BAS\"\nDim synth As New DX7(17)",path));assert(has("outside requested"));
    assert(run("Imports \"DX7.BAS\"\nDim synth As New DX7(1)\nsynth.OperatorLevel(7,50)",path));
    assert(!run("Imports \"DX7.BAS\"\nDim synth As New DX7(1)\nMidi.EnableOutput(True)\nsynth.NoteOn(60,96)\nsynth.SetChannel(2)\nsynth.NoteOn(62,96)",path));
    assert(packet_count==4);packet(1,128,60,0,3);packet(3,129,62,0,3);
    assert(!demo(argv[1],0,0));assert(!packet_count);
    assert(!demo(argv[1],1,0));assert(packet_count==6);packet(0,144,60,96,3);packet(4,128,60,0,3);packet(5,192,0,0,2);
    assert(packets[1][3]==1 && packets[1][4]==6 && packets[1][5]==0);
    assert(demo(argv[1],1,1));assert(packet_count==2);packet(1,128,60,0,3);assert(has("outside requested"));
    arturia_test=1;assert(!demo(argv[1],1,0));assert(packet_count==3);
    packet(1,176,18,127,3);packet(2,128,60,0,3);
    arturia_test=2;assert(!demo(argv[1],1,0));assert(packet_count==3);
    packet(1,176,18,0,3);
    assert(!demo(argv[1],0,0));assert(!packet_count);
    midi_set_writer(0);printf("%d MIDI/cleanup/DX7 checks passed.\n",checks);return 0;
}
