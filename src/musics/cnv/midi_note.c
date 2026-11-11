#ifndef zoxm_musics_midi_note
#define zoxm_musics_midi_note
#define MAX_NOTES 128
// music .zox: a uint32 note count, then the notes (written by the midi converter, bsh/convert.sh --music)
#include <stdint.h>
// the converter builds on its own (without the engine)
#ifndef zox_logv
    #define zox_logv(...) { }
#endif

typedef struct {
    double time;
    double length;
    double frequency;
} MidiNote;

#endif
