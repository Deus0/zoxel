// Jingles: short tunes for memorable moments (a level up, a quest taken, a quest handed in)
//  - zelda's lift, metroid's mystery: quick rising arpeggios in minor modes, landing on open, suspended
//      chords (no third, or a ninth on top), a bell and a choir pad over a low root, sparkles above
//  - echoes repeat the bright notes quieter and later, like a cave
//  - each note is a sound waiting its start (SoundDelay), the mixer times them to the sample
//  - notes are semitones from the jingle's tonic, played in the middle (2D), they're the player's moments

typedef struct {
    float start;        // seconds from the jingle's start
    float seconds;
    sbyte note;         // semitones from the tonic
    byte instrument;
    float volume;
} JingleNote;

#define jingle_max_notes 24
#define jingle_echo_seconds 0.19f
#define jingle_echo_volume 0.35f
#define jingle_volume 1.15f          // of the sfx volume, a moment stands out

typedef struct {
    const char* name;
    byte tonic;         // note_frequencies index
    byte echo;          // echoes its bright notes (music box, bell)
    byte count;
    JingleNote notes[jingle_max_notes];
} Jingle;

typedef enum {
    jingle_level_up,
    jingle_quest_taken,
    jingle_quest_handed_in,
    jingle_end
} JingleType;

const Jingle jingles[jingle_end] = {
    // a rising aeolian arpeggio, root fifth octave ninth, then an open sus2 chord rings out
    [jingle_level_up] = { "level up", 45, 1, 12, {
        { 0.00f, 0.30f, 0, instrument_harp, 0.8f },
        { 0.09f, 0.30f, 7, instrument_harp, 0.8f },
        { 0.18f, 0.30f, 12, instrument_harp, 0.85f },
        { 0.27f, 0.40f, 14, instrument_music_box, 0.7f },
        { 0.36f, 0.40f, 19, instrument_music_box, 0.75f },
        // the landing: a deep root, an open chord held by the choir, a bell on top
        { 0.48f, 2.60f, -12, instrument_cello, 0.7f },
        { 0.48f, 2.40f, 0, instrument_choir, 0.55f },
        { 0.50f, 2.40f, 7, instrument_choir, 0.5f },
        { 0.52f, 2.40f, 14, instrument_choir, 0.45f },
        { 0.48f, 2.60f, 12, instrument_bell, 0.8f },
        { 0.62f, 1.20f, 26, instrument_music_box, 0.5f },
        { 0.80f, 1.20f, 31, instrument_music_box, 0.4f },
    } },
    // a question: two notes up a fourth, an open fifth left hanging (no third)
    [jingle_quest_taken] = { "quest taken", 50, 1, 6, {
        { 0.00f, 0.35f, 0, instrument_harp, 0.75f },
        { 0.12f, 0.50f, 5, instrument_harp, 0.8f },
        { 0.26f, 1.40f, 7, instrument_bell, 0.6f },
        { 0.26f, 1.40f, -5, instrument_cello, 0.5f },
        { 0.27f, 1.30f, 0, instrument_choir, 0.4f },
        { 0.40f, 0.80f, 17, instrument_music_box, 0.45f },
    } },
    // the answer: a dorian turn that resolves, minor to a bright major sixth, a full chord under a bell
    [jingle_quest_handed_in] = { "quest handed in", 47, 1, 11, {
        { 0.00f, 0.25f, 0, instrument_harp, 0.8f },
        { 0.08f, 0.25f, 3, instrument_harp, 0.8f },
        { 0.16f, 0.25f, 7, instrument_harp, 0.8f },
        { 0.24f, 0.30f, 9, instrument_harp, 0.85f },
        { 0.32f, 0.40f, 12, instrument_music_box, 0.75f },
        { 0.44f, 2.60f, -12, instrument_cello, 0.7f },
        { 0.44f, 2.40f, 0, instrument_choir, 0.5f },
        { 0.46f, 2.40f, 4, instrument_choir, 0.45f },
        { 0.48f, 2.40f, 9, instrument_choir, 0.45f },
        { 0.44f, 2.60f, 16, instrument_bell, 0.75f },
        { 0.60f, 1.20f, 24, instrument_music_box, 0.5f },
    } },
};

// the jingle's notes with their echoes, in time order, returns how many
int jingle_performed(JingleType type, JingleNote* out, int max) {
    if (type >= jingle_end) {
        return 0;
    }
    const Jingle* jingle = &jingles[type];
    int count = 0;
    for (int i = 0; i < jingle->count && count < max; i++) {
        const JingleNote note = jingle->notes[i];
        out[count++] = note;
        const byte bright = note.instrument == instrument_music_box || note.instrument == instrument_bell || note.instrument == instrument_harp;
        if (jingle->echo && bright && count < max) {
            JingleNote echo = note;
            echo.start += jingle_echo_seconds;
            echo.volume *= jingle_echo_volume;
            out[count++] = echo;
        }
    }
    // in order of start
    for (int i = 1; i < count; i++) {
        const JingleNote note = out[i];
        int j = i - 1;
        while (j >= 0 && out[j].start > note.start) {
            out[j + 1] = out[j];
            j--;
        }
        out[j + 1] = note;
    }
    return count;
}

// how long a jingle rings, in seconds
float jingle_seconds(JingleType type) {
    JingleNote notes[jingle_max_notes * 2];
    const int count = jingle_performed(type, notes, jingle_max_notes * 2);
    float end = 0;
    for (int i = 0; i < count; i++) {
        end = fmaxf(end, notes[i].start + notes[i].seconds);
    }
    return end;
}

// plays a jingle, each note a sound waiting its start
void spawn_jingle(ecs* world, JingleType type) {
    if (nosounds || type >= jingle_end) {
        return;
    }
    JingleNote notes[jingle_max_notes * 2];
    const int count = jingle_performed(type, notes, jingle_max_notes * 2);
    const int tonic = jingles[type].tonic;
    for (int i = 0; i < count; i++) {
        const JingleNote note = notes[i];
        const int index = tonic + note.note;
        if (index <= 0 || index >= notes_max) {
            continue;
        }
        const float seconds = fminf(note.seconds, 2.8f);
        const entity e = spawn_sound_generated(world, prefab_sound_generated, note.instrument, note_frequencies[index],
            seconds, note.volume * jingle_volume * get_volume_sfx());
        if (e && note.start > 0) {
            zox_set(e, SoundDelay, { note.start });
        }
    }
}
