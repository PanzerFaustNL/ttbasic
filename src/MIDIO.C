/* Atari BIOS device 3 is the built-in MIDI output. */
#include <tos.h>
#include "MIDIO.H"
int tos_midi_write(unsigned char *bytes,int count)
{
    int i;for(i=0;i<count;++i)Bconout(3,(int)bytes[i]);return 1;
}
