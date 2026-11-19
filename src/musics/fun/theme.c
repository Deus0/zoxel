// The menu theme: a slow, dark synth piece, new every time it's composed, the same at heart
//  - the call: two bars every theme plays, the same intervals (up a fifth, the octave, falling to a minor
//      sixth that sinks back), what you remember
//  - the rest from its seed: the key, a dark mode, the tempo, the chords under the call, the answer to it
//      (two bars, left open the first time, home the second), the arpeggio's pattern, the floor plan
//  - sections: intro (wind, a pad, the arpeggio coming in), main (the lead plays call and answer), echo
//      (glass plays them over a choir), rise (all of it, a pulsing bass, a choir under the lead, bells),
//      outro (it thins out, the call's start on glass, home)
//  - the synths are in sounds/bop/synths.c, steps are sixteenths, bars 16 steps
//  - bright notes echo a dotted eighth later, quieter, like a big empty room
#define theme_steps_per_bar 16
#define theme_echo_steps 3
#define theme_echo_volume 0.3f
#define theme_role_intro 0
#define theme_role_main 1
#define theme_role_echo 2
#define theme_role_rise 3
#define theme_role_outro 4

// a melody note: semitones from the tonic, its step in its phrase and how many steps it lasts
typedef struct {
    sbyte note;
    byte step;
    byte steps;
} ThemeNote;

// the call, the same in every theme
#define theme_call_count 8
const ThemeNote theme_call[theme_call_count] = {
    { 0, 0, 6 }, { 7, 6, 2 }, { 12, 8, 6 }, { 10, 14, 1 }, { 7, 15, 1 },
    { 8, 16, 10 }, { 7, 26, 2 }, { 3, 28, 4 },
};

// a chord: its root (semitones from the tonic), minor or major
typedef struct {
    sbyte root;
    byte minor;
} ThemeChord;

// dark modes, all with a minor third and sixth, the call fits each
#define theme_modes_count 3
const byte theme_modes[theme_modes_count][7] = {
    { 0, 2, 3, 5, 7, 8, 10 },   // aeolian
    { 0, 1, 3, 5, 7, 8, 10 },   // phrygian
    { 0, 2, 3, 5, 7, 8, 11 },   // harmonic minor
};

// under the call's second bar, both hold its minor sixth and the octave: VI or iv
const ThemeChord theme_call_chords[2] = { { 8, 0 }, { 5, 1 } };

// under the answer's two bars, left open (the call comes back after)
#define theme_answer_chords_count 5
const ThemeChord theme_answer_chords[theme_answer_chords_count][2] = {
    { { 3, 0 }, { 10, 0 } },    // III VII
    { { 5, 1 }, { 7, 0 } },     // iv V
    { { 1, 0 }, { 7, 0 } },     // II V, phrygian
    { { 8, 1 }, { 10, 0 } },    // vi (a dark shift) VII
    { { 3, 0 }, { 1, 0 } },     // III II
};

// arpeggios over a chord's tones (0 root, 1 third, 2 fifth, 3 octave, 4 tenth)
#define theme_arpeggios_count 6
const byte theme_arpeggios[theme_arpeggios_count][8] = {
    { 0, 1, 2, 3, 4, 3, 2, 1 },
    { 0, 2, 1, 3, 2, 4, 3, 1 },
    { 0, 1, 2, 1, 3, 2, 4, 2 },
    { 4, 3, 2, 1, 0, 1, 2, 3 },
    { 0, 2, 3, 2, 4, 2, 3, 2 },
    { 0, 3, 1, 4, 2, 3, 1, 3 },
};

// floor plans, 255 ends one
#define theme_forms_count 3
const byte theme_forms[theme_forms_count][7] = {
    { theme_role_intro, theme_role_main, theme_role_echo, theme_role_rise, theme_role_outro, 255 },
    { theme_role_intro, theme_role_main, theme_role_rise, theme_role_echo, theme_role_main, theme_role_outro, 255 },
    { theme_role_intro, theme_role_echo, theme_role_main, theme_role_rise, theme_role_outro, 255 },
};

typedef struct {
    Song* song;
    uint rng;
    byte mode;
    uint steps;                 // the whole theme's
    ThemeChord chords[8];       // its eight bars: call, answer (open), call, answer (home)
    ThemeNote answer[2][8];     // open, home
    byte answer_count;
} Theme;

static inline int theme_chord_tone(ThemeChord chord, int root, int tone) {
    const int third = chord.minor ? 3 : 4;
    const int tones[5] = { 0, third, 7, 12, 12 + third };
    return root + tones[tone];
}

// is a note (semitones from the tonic) one of the chord's
static inline byte theme_in_chord(ThemeChord chord, int note) {
    const int interval = ((note - chord.root) % 12 + 12) % 12;
    return interval == 0 || interval == 7 || interval == (chord.minor ? 3 : 4);
}

// the nearest of the chord's notes
static inline int theme_snap(ThemeChord chord, int note) {
    for (int offset = 0; offset < 6; offset++) {
        if (theme_in_chord(chord, note - offset)) {
            return note - offset;
        }
        if (theme_in_chord(chord, note + offset)) {
            return note + offset;
        }
    }
    return note;
}

// the next note in the mode, up (1) or down (-1)
static inline int theme_step(byte mode, int note, int direction) {
    for (int i = 1; i <= 3; i++) {
        const int semitone = ((note + i * direction) % 12 + 12) % 12;
        for (int d = 0; d < 7; d++) {
            if (theme_modes[mode][d] == semitone) {
                return note + i * direction;
            }
        }
    }
    return note;
}

// a root into the octave around the tonic, -5 to 6 semitones
static inline int theme_near(int note) {
    note = (note % 12 + 12) % 12;
    return note > 6 ? note - 12 : note;
}

// a note at a step (sixteenths from the start), none past the theme's end
static inline void theme_note(Theme* t, float step, float steps, int note, byte instrument, byte voice, float volume) {
    if (step >= t->steps) {
        return;
    }
    const float step_seconds = t->song->step_seconds;
    song_add_event(t->song, step * step_seconds, steps * step_seconds, t->song->tonic + note, instrument, voice, volume);
}

// a note and its echo
static inline void theme_note_echoed(Theme* t, float step, float steps, int note, byte instrument, byte voice, float volume) {
    theme_note(t, step, steps, note, instrument, voice, volume);
    theme_note(t, step + theme_echo_steps, steps, note, instrument, voice, volume * theme_echo_volume);
}

// the answer: notes through its first bar, mostly falling stepwise, strong beats on the chord, then a note or
//  two into its second bar and one held to its end, on the open chord, or home (the tonic) the second time
static inline void theme_compose_answer(Theme* t) {
    static const byte lengths[] = { 2, 2, 4, 4, 6 };
    int note = theme_snap(t->chords[2], 5);
    int step = 0;
    byte count = 0;
    while (step < 22 && count < 7) {
        int steps = lengths[song_rand(&t->rng) % 5];
        if (step < 16 && step + steps > 16) {
            steps = 16 - step;
        } else if (step >= 16) {
            steps = 2 + 2 * (song_rand(&t->rng) % 2);
        }
        t->answer[0][count++] = (ThemeNote) { (sbyte) note, (byte) step, (byte) steps };
        step += steps;
        const ThemeChord chord = t->chords[2 + step / 16];
        const uint move = song_rand(&t->rng) % 6;
        if (move == 0) {
            note = theme_snap(chord, note + 5);
        } else if (move == 1) {
            note = theme_snap(chord, note - 4);
        } else {
            note = theme_step(t->mode, note, move < 4 ? -1 : 1);
        }
        if (step % 8 == 0) {
            note = theme_snap(chord, note);
        }
        if (note > 14) {
            note = theme_snap(chord, note - 7);
        } else if (note < -2) {
            note = theme_snap(chord, note + 7);
        }
    }
    t->answer[0][count++] = (ThemeNote) { (sbyte) theme_snap(t->chords[3], note), (byte) step, (byte) (32 - step) };
    // home: its first bar the same, its second on the tonic chord, landing on the tonic
    const ThemeChord home = t->chords[7];
    for (byte i = 0; i < count; i++) {
        ThemeNote n = t->answer[0][i];
        if (n.step >= 16) {
            n.note = (sbyte) theme_snap(home, n.note);
        }
        t->answer[1][i] = n;
    }
    const int last = t->answer[1][count - 1].note;
    t->answer[1][count - 1].note = abs(last - 12) < abs(last) ? 12 : 0;
    t->answer_count = count;
}

// the eight bar melody from a bar: call, answer, call, answer home
static inline void theme_melody(Theme* t, int bar, byte instrument, float volume, int octave, byte min_steps) {
    for (int half = 0; half < 2; half++) {
        const int start = (bar + half * 4) * theme_steps_per_bar;
        for (int i = 0; i < theme_call_count; i++) {
            const ThemeNote n = theme_call[i];
            if (n.steps >= min_steps) {
                const float accent = n.step % 8 == 0 ? 1 : 0.85f;
                theme_note_echoed(t, start + n.step, n.steps, n.note + octave, instrument, song_voice_melody, volume * accent);
            }
        }
        for (int i = 0; i < t->answer_count; i++) {
            const ThemeNote n = t->answer[half][i];
            if (n.steps >= min_steps) {
                const float accent = n.step % 8 == 0 ? 1 : 0.85f;
                theme_note_echoed(t, start + 32 + n.step, n.steps, n.note + octave, instrument, song_voice_melody, volume * accent);
            }
        }
    }
}

// the call's first three notes (its fifth and octave), a reminder
static inline void theme_call_start(Theme* t, int bar, byte instrument, float volume) {
    for (int i = 0; i < 3; i++) {
        const ThemeNote n = theme_call[i];
        theme_note_echoed(t, bar * theme_steps_per_bar + n.step, n.steps, n.note, instrument, song_voice_melody, volume);
    }
}

// a chord held through a bar, in two overlapping swells, gently strummed, an octave under the tonic
static inline void theme_pad(Theme* t, int bar, ThemeChord chord, byte instrument, float volume) {
    const int root = theme_near(chord.root) - 12;
    for (int half = 0; half < 2; half++) {
        for (int tone = 0; tone < 3; tone++) {
            const float step = bar * theme_steps_per_bar + half * 8 + tone * 0.15f;
            theme_note(t, step, 12, theme_chord_tone(chord, root, tone), instrument, song_voice_pad, volume);
        }
    }
}

// the chord's root two octaves down: held in halves, or pulsing eighths with an octave jump
static inline void theme_bass(Theme* t, int bar, ThemeChord chord, byte pulse, float volume) {
    const int root = theme_near(chord.root) - 24;
    const int start = bar * theme_steps_per_bar;
    if (!pulse) {
        theme_note(t, start, 8.5f, root, instrument_synth_bass, song_voice_bass, volume);
        theme_note(t, start + 8, 8.5f, root, instrument_synth_bass, song_voice_bass, volume * 0.9f);
        return;
    }
    for (int i = 0; i < 8; i++) {
        const int note = root + (i == 3 || i == 7 ? 12 : 0);
        theme_note(t, start + i * 2, 1.6f, note, instrument_synth_bass, song_voice_bass, volume * (i % 2 ? 0.8f : 1));
    }
}

// glass up and down the chord, every rate steps
static inline void theme_arpeggio(Theme* t, int bar, ThemeChord chord, byte pattern, int rate, int octave, float volume, byte echo) {
    const int root = theme_near(chord.root) + octave;
    for (int i = 0; i < theme_steps_per_bar; i += rate) {
        const int note = theme_chord_tone(chord, root, theme_arpeggios[pattern][(i / rate) % 8]);
        const float step = bar * theme_steps_per_bar + i;
        const float loud = volume * (i % 4 == 0 ? 1 : 0.8f);
        if (echo) {
            theme_note_echoed(t, step, 3, note, instrument_synth_glass, song_voice_arpeggio, loud);
        } else {
            theme_note(t, step, 3, note, instrument_synth_glass, song_voice_arpeggio, loud);
        }
    }
}

// wind through the bar, on the tonic or its fifth
static inline void theme_wind(Theme* t, int bar, int note, float volume) {
    theme_note(t, bar * theme_steps_per_bar, 12, note, instrument_synth_wind, song_voice_pad, volume);
}

static inline void theme_section(Theme* t, byte role, int bar) {
    const byte pattern = song_rand(&t->rng) % theme_arpeggios_count;
    switch (role) {
        case theme_role_intro:
            // wind and a pad, the arpeggio coming in, the call's start on glass
            for (int b = 0; b < 4; b++) {
                const ThemeChord chord = t->chords[b % 2];
                theme_wind(t, bar + b, b % 2 ? -5 : -12, 0.3f);
                theme_pad(t, bar + b, chord, instrument_synth_pad, 0.16f + 0.04f * b);
                if (b > 0) {
                    theme_arpeggio(t, bar + b, chord, pattern, 2, -12, 0.1f + 0.05f * b, 0);
                }
            }
            theme_call_start(t, bar + 2, instrument_synth_glass, 0.55f);
            break;
        case theme_role_main: {
            // sometimes glass doubles the lead an octave up, quietly
            const byte doubled = song_rand(&t->rng) % 3 == 0;
            theme_melody(t, bar, instrument_synth_lead, 0.8f, 0, 0);
            if (doubled) {
                theme_melody(t, bar, instrument_synth_glass, 0.25f, 12, 4);
            }
            for (int b = 0; b < 8; b++) {
                theme_pad(t, bar + b, t->chords[b], instrument_synth_pad, 0.24f);
                theme_bass(t, bar + b, t->chords[b], 0, 0.5f);
                theme_arpeggio(t, bar + b, t->chords[b], pattern, 2, -12, 0.2f, 0);
            }
            break;
        }
        case theme_role_echo:
            // glass plays the melody over a choir, the bass comes in halfway
            theme_melody(t, bar, instrument_synth_glass, 0.65f, 0, 0);
            for (int b = 0; b < 8; b++) {
                theme_pad(t, bar + b, t->chords[b], instrument_choir, 0.2f);
                if (b >= 4) {
                    theme_bass(t, bar + b, t->chords[b], 0, 0.4f);
                }
                if (b % 4 == 0) {
                    theme_wind(t, bar + b, -12, 0.22f);
                }
            }
            break;
        case theme_role_rise:
            // everything: the lead with a choir under it, sixteenths of glass, a pulsing bass, bells
            theme_melody(t, bar, instrument_synth_lead, 0.85f, 0, 0);
            theme_melody(t, bar, instrument_choir, 0.3f, -12, 4);
            for (int b = 0; b < 8; b++) {
                theme_pad(t, bar + b, t->chords[b], instrument_synth_pad, 0.26f);
                theme_bass(t, bar + b, t->chords[b], 1, 0.5f);
                theme_arpeggio(t, bar + b, t->chords[b], pattern, 1, 0, 0.16f, b % 2);
                if (b % 4 == 0) {
                    theme_note(t, (bar + b) * theme_steps_per_bar, 12, theme_near(t->chords[b].root) - 12,
                        instrument_bell, song_voice_pad, 0.4f);
                }
            }
            break;
        default: {
            // outro: the call's start, then home, it fades
            const ThemeChord home = t->chords[0];
            theme_call_start(t, bar, instrument_synth_glass, 0.5f);
            for (int b = 0; b < 3; b++) {
                const ThemeChord chord = b == 1 ? t->chords[1] : home;
                theme_pad(t, bar + b, chord, instrument_synth_pad, 0.22f - 0.05f * b);
                theme_wind(t, bar + b, b % 2 ? -5 : -12, 0.25f);
            }
            theme_note_echoed(t, (bar + 2) * theme_steps_per_bar, 12, 0, instrument_synth_glass, song_voice_melody, 0.45f);
            theme_note(t, (bar + 2) * theme_steps_per_bar, 12, -12, instrument_bell, song_voice_pad, 0.35f);
            break;
        }
    }
}

void compose_menu_theme(uint seed, Song* song) {
    memset(song, 0, sizeof(Song));
    Theme t = { .song = song, .rng = seed ? seed : 1 };
    t.mode = song_rand(&t.rng) % theme_modes_count;
    song->mode = t.mode;
    song->tonic = 50 + song_rand(&t.rng) % 7;     // D4 to G#4
    const float bar_seconds = 3.3f + 0.6f * (song_rand(&t.rng) % 100) / 100.0f;
    song->step_seconds = bar_seconds / theme_steps_per_bar;
    // the chords: home, under the call, the answer, home and the call again, the answer turning home
    const ThemeChord tonic = { 0, 1 };
    const ThemeChord call = theme_call_chords[song_rand(&t.rng) % 2];
    const ThemeChord* answer = theme_answer_chords[song_rand(&t.rng) % theme_answer_chords_count];
    const ThemeChord chords[8] = { tonic, call, answer[0], answer[1], tonic, call, answer[0], tonic };
    memcpy(t.chords, chords, sizeof(chords));
    theme_compose_answer(&t);
    // the floor plan
    const byte* form = theme_forms[song_rand(&t.rng) % theme_forms_count];
    int bars = 0;
    for (int i = 0; form[i] != 255; i++) {
        const byte bars_of = form[i] == theme_role_intro || form[i] == theme_role_outro ? 4 : 8;
        song->sections[i] = (SongSection) { form[i], 0, 0, bars_of, (byte) bars };
        song->sections_count++;
        bars += bars_of;
    }
    t.steps = bars * theme_steps_per_bar;
    song->steps_count = t.steps;
    for (int i = 0; i < song->sections_count; i++) {
        theme_section(&t, song->sections[i].role, song->sections[i].start_bar);
    }
    song_sort_events(song);
    song->seconds = t.steps * song->step_seconds;
}
