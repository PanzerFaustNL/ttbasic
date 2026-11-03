/* MIDI 1.0 messages, dry-run logging and owned-note cleanup. No GEM dependency. */
#include <stdio.h>
#include <string.h>
#include "MIDI.H"
static MidiWriter output;
static MidiLogger logger;
static int enabled;
static unsigned char notes[16][16],sustain[16];
static char last[MIDI_TEXT],problem[80];
void midi_set_writer(MidiWriter writer){output=writer;}
void midi_set_logger(MidiLogger callback){logger=callback;}
int midi_enabled(void){return enabled;}
char *midi_last_message(void){return last;}
char *midi_last_error(void){return problem;}
static int error(char *message){strncpy(problem,message,79);problem[79]=0;return 0;}
static int channel(long value){return value>=1 && value<=16?1:error("MIDI channel must be 1..16");}
static int data(long value){return value>=0 && value<=127?1:error("MIDI data must be 0..127");}
static void info(char *message){strncpy(last,message,MIDI_TEXT-1);last[MIDI_TEXT-1]=0;if(logger)logger(last);}
static int send(unsigned char *bytes,int count)
{
    int i,pos;char item[4];
    if(enabled && (!output || !output(bytes,count)))return error("MIDI output failed");
    strcpy(last,enabled?"TX: ":"DRY: ");pos=strlen(last);
    for(i=0;i<count && pos+4<MIDI_TEXT-4;++i){sprintf(item,"%02X",(unsigned)bytes[i]);if(i)last[pos++]=' ';memcpy(last+pos,item,2);pos+=2;last[pos]=0;}
    if(i<count)strcat(last," ...");if(logger)logger(last);return 1;
}
static int voice(long ch,int status,long a,long b,int count)
{
    unsigned char bytes[3];
    if(!channel(ch) || !data(a) || (count==3 && !data(b)))return 0;
    bytes[0]=(unsigned char)(status+(int)ch-1);bytes[1]=(unsigned char)a;bytes[2]=(unsigned char)b;return send(bytes,count);
}
int midi_note_on(long ch,long note,long velocity)
{
    if(!voice(ch,0x90,note,velocity,3))return 0;
    if(velocity)notes[(int)ch-1][(int)note/8]|=(unsigned char)(1U<<((int)note%8));
    else notes[(int)ch-1][(int)note/8]&=(unsigned char)~(1U<<((int)note%8));return 1;
}
int midi_note_off(long ch,long note,long velocity)
{
    if(!voice(ch,0x80,note,velocity,3))return 0;
    notes[(int)ch-1][(int)note/8]&=(unsigned char)~(1U<<((int)note%8));return 1;
}
int midi_program(long ch,long program){return voice(ch,0xC0,program,0,2);}
int midi_control(long ch,long controller,long value)
{
    if(!voice(ch,0xB0,controller,value,3))return 0;
    if(controller==64)sustain[(int)ch-1]=value>=64;return 1;
}
int midi_pitch(long ch,long value)
{
    if(value<0 || value>16383)return error("Pitch bend must be 0..16383 (centre 8192)");
    return voice(ch,0xE0,value%128,value/128,3);
}
int midi_sysex(unsigned char *bytes,int count)
{
    int i;if(count<2 || count>MIDI_PACKET || bytes[0]!=0xF0 || bytes[count-1]!=0xF7)return error("SysEx must contain F0 ... F7, max 128 bytes");
    for(i=1;i<count-1;++i)if(bytes[i]>127)return error("SysEx payload must contain 7-bit data");return send(bytes,count);
}
int midi_stop_notes(long channel_number)
{
    int ch,note,ok;if(!channel(channel_number))return 0;ch=(int)channel_number-1;ok=1;
    if(sustain[ch] && !midi_control(channel_number,64,0))ok=0;
    for(note=0;note<128;++note)if(notes[ch][note/8] & (1U<<(note%8)))if(!midi_note_off(channel_number,note,0))ok=0;
    return ok;
}
int midi_silence(void)
{
    int ch,ok;ok=1;for(ch=1;ch<=16;++ch)if(!midi_stop_notes(ch))ok=0;return ok;
}
int midi_panic(long ch)
{
    int note;if(!channel(ch))return 0;
    if(!midi_control(ch,64,0))return 0;
    for(note=0;note<128;++note)if(!midi_note_off(ch,note,0))return 0;
    info("PANIC: sustain off and 128 note-offs sent");return 1;
}
int midi_enable(int on)
{
    on=on!=0;if(on==enabled)return 1;
    if(on && !output)return error("MIDI output is not available on this host");
    if(!midi_silence())return 0;
    enabled=on;info(enabled?"MIDI OUT enabled (no connection detection)":"DRY RUN: no MIDI output");return 1;
}
void midi_begin(void)
{
    enabled=0;memset(notes,0,sizeof(notes));memset(sustain,0,sizeof(sustain));problem[0]=0;
    strcpy(last,"DRY RUN: no MIDI output");
}
int midi_finish(void)
{
    int ok;ok=midi_silence();enabled=0;return ok;
}
