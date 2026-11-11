#include <stdio.h>
#include <math.h>
#include "midi_note.c"


// Save the notes to a file
int save_notes_to_file(MidiNote *notes, int note_count, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for saving");
        return -1;
    }
    const uint32_t count = (uint32_t) note_count;
    fwrite(&count, sizeof(uint32_t), 1, file); // the number of notes
    fwrite(notes, sizeof(MidiNote), note_count, file); // Write the notes data
    /*for (int i = 0; i < note_count; i++) {
        MidiNote note = notes[i];
        fwrite(&note.time, sizeof(double), 1, file);
        fwrite(&note.length, sizeof(double), 1, file);
        fwrite(&note.frequency, sizeof(double), 1, file);
    }*/
    fclose(file);
    return 0;
}
