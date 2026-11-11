// #include <stdio.h>
// #include <math.h>

// Load the notes from a file
// Loads up to MAX_NOTES notes, returns how many, 0 if it failed
//  - the count is a uint32, the saver's int (it was read as a size_t, 8 bytes on 64 bit,
//      so the count took half the first note and the music never loaded)
int load_notes_from_file(MidiNote *notes, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file for loading");
        return 0;
    }
    uint32_t note_count = 0;
    if (fread(&note_count, sizeof(uint32_t), 1, file) != 1) {
        zox_logv("[load_notes_from_file] no note count in [%s]", filename);
        fclose(file);
        return 0;
    }
    if (note_count > MAX_NOTES) {
        zox_logv("[load_notes_from_file] [%u] notes, keeping [%i]", note_count, MAX_NOTES);
        note_count = MAX_NOTES;
    }
    const size_t items_read = fread(notes, sizeof(MidiNote), note_count, file);
    fclose(file);
    if (items_read < note_count) {
        zox_logv("[load_notes_from_file] read [%zu] of [%u] notes", items_read, note_count);
    }
    return (int) items_read;
}

void test_notes_from_file(const char *filename) {
    // Load notes from file (for testing the save/load functionality)
    MidiNote loaded_notes[MAX_NOTES];
    int loaded_note_count = load_notes_from_file(loaded_notes, filename);
    if (loaded_note_count > 0) {
        printf("Notes loaded from %s:\n", filename);
        for (int i = 0; i < loaded_note_count; i++) {
            printf("MidiNote %d - Frequency: %.2f Hz, Start time: %.2f, Length: %.2f\n", i + 1, loaded_notes[i].frequency, loaded_notes[i].time, loaded_notes[i].length);
        }
    } else {
        printf("loaded_note_count is 0.\n");
    }
}

// notes = (MidiNote*) malloc(sizeof(MidiNote) * note_count);
/*for (int i = 0; i < note_count; i++) {
 * MidiNote note;
 * fread(&note.time, sizeof(double), 1, file);
 * fread(&note.length, sizeof(double), 1, file);
 * fread(&note.frequency, sizeof(double), 1, file);
 * notes[i] = note;
 } */

