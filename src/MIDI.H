#ifndef TT_MIDI_H
#define TT_MIDI_H
#define MIDI_PACKET 128
#define MIDI_TEXT 120
typedef int (*MidiWriter)(unsigned char *bytes,int count);
typedef void (*MidiLogger)(char *message);
void midi_set_writer(MidiWriter writer);
void midi_set_logger(MidiLogger logger);
void midi_begin(void);
int midi_finish(void);
int midi_silence(void);
int midi_stop_notes(long channel);
int midi_enable(int enabled);
int midi_enabled(void);
int midi_note_on(long channel,long note,long velocity);
int midi_note_off(long channel,long note,long velocity);
int midi_program(long channel,long program);
int midi_control(long channel,long controller,long value);
int midi_pitch(long channel,long value);
int midi_sysex(unsigned char *bytes,int count);
int midi_panic(long channel);
char *midi_last_message(void);
char *midi_last_error(void);
#endif
