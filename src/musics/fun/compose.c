// Composes a song like a dungeon is generated
//  - the form is the floor plan: sections (rooms) in order, i.e. intro A A' B A outro
//  - motifs are room templates: a section reuses its motif, varied when it comes back (A')
//  - chord progressions are the corridors: a walk through a chord graph, each section ending
//      on a cadence (the last one on the tonic)
//  - layers are the decoration: melody, bass, arpeggio and pad, by the section's role
//  - deterministic from its seed, steps are eighth notes, bars 8 steps
#define song_max_steps 640
#define song_max_sections 14
#define song_steps_per_bar 8
#define song_voices 4
#define song_voice_melody 0
#define song_voice_bass 1
#define song_voice_arpeggio 2
#define song_voice_pad 3
#define song_motif_steps 16         // two bars
#define song_max_motifs 4

#define song_role_intro 0
#define song_role_theme 1
#define song_role_contrast 2
#define song_role_climax 3
#define song_role_outro 4

typedef struct {
    byte note;          // note_frequencies index, 0 is a rest
    byte instrument;
    byte steps;         // how many steps it lasts
    float volume;
} SongNote;

typedef struct {
    SongNote voices[song_voices];
} SongStep;

typedef struct {
    byte role;
    byte motif;         // which motif (room template) its melody uses
    byte variation;     // 0 the motif as is, otherwise varied (A')
    byte bars;
    byte start_bar;
} SongSection;

// a motif: per step, an onset (or not), its scale degree offset and length
typedef struct {
    byte onset[song_motif_steps];
    sbyte degree[song_motif_steps];
    byte length[song_motif_steps];
} SongMotif;

typedef struct {
    uint steps_count;
    SongStep steps[song_max_steps];
    float step_seconds;
    byte tonic;             // melody octave tonic, note index
    byte mode;
    byte sections_count;
    SongSection sections[song_max_sections];
    SongMotif motifs[song_max_motifs];
    byte chords[song_max_steps / song_steps_per_bar];   // chord degree (0 to 6) per bar
    // the ensemble (picked from the seed)
    byte theme_lead;        // the music's instrument
    byte contrast_lead;     // contrast sections, and the varied themes (A')
    byte bass;
    byte arpeggio;
    byte pad;
} Song;

// modes, as semitones per scale degree
#define song_modes_count 6
const byte song_modes[song_modes_count][7] = {
    { 0, 2, 3, 5, 7, 8, 10 },   // aeolian (minor)
    { 0, 2, 3, 5, 7, 9, 10 },   // dorian
    { 0, 1, 3, 5, 7, 8, 10 },   // phrygian
    { 0, 2, 3, 5, 7, 8, 11 },   // harmonic minor
    { 0, 2, 4, 5, 7, 9, 11 },   // major
    { 0, 2, 4, 6, 7, 9, 11 },   // lydian
};
// melancholic modes are likelier
const byte song_mode_weights[song_modes_count] = { 5, 4, 2, 2, 2, 1 };

// forms (floor plans), roles with motifs, 255 ends it
typedef struct {
    byte role;
    byte motif;
    byte variation;
    byte bars;
} SongFormSection;
#define song_forms_count 3
const SongFormSection song_forms[song_forms_count][song_max_sections] = {
    {   // a ballad: intro A A' B A' C B' A outro
        { song_role_intro, 0, 0, 4 }, { song_role_theme, 0, 0, 4 }, { song_role_theme, 0, 1, 4 },
        { song_role_contrast, 1, 0, 4 }, { song_role_theme, 0, 1, 4 }, { song_role_climax, 2, 0, 4 },
        { song_role_contrast, 1, 1, 4 }, { song_role_theme, 0, 0, 4 }, { song_role_outro, 0, 2, 4 },
        { 255 }
    },
    {   // a journey: intro A B A' B' C C' A B outro
        { song_role_intro, 0, 0, 2 }, { song_role_theme, 0, 0, 4 }, { song_role_contrast, 1, 0, 4 },
        { song_role_theme, 0, 1, 4 }, { song_role_contrast, 1, 1, 4 }, { song_role_climax, 2, 0, 4 },
        { song_role_climax, 2, 1, 4 }, { song_role_theme, 0, 0, 4 }, { song_role_contrast, 1, 0, 4 },
        { song_role_outro, 0, 2, 2 }, { 255 }
    },
    {   // a wander: A A' B A C A' B' C' A outro
        { song_role_theme, 0, 0, 4 }, { song_role_theme, 0, 1, 4 }, { song_role_contrast, 1, 0, 4 },
        { song_role_theme, 0, 0, 4 }, { song_role_climax, 2, 0, 4 }, { song_role_theme, 0, 1, 4 },
        { song_role_contrast, 1, 1, 4 }, { song_role_climax, 2, 1, 4 }, { song_role_theme, 0, 0, 4 },
        { song_role_outro, 0, 2, 2 }, { 255 }
    },
};

// chord graph (corridors), from a chord degree to the next ones, weighted
const byte song_chord_next[7][4] = {
    { 3, 5, 6, 4 },     // i -> iv VI VII v
    { 4, 6, 0, 0 },     // ii -> v VII
    { 5, 3, 0, 0 },     // III -> VI iv
    { 0, 6, 4, 1 },     // iv -> i VII v ii
    { 0, 5, 0, 0 },     // v -> i VI
    { 3, 6, 2, 0 },     // VI -> iv VII III
    { 0, 2, 5, 0 },     // VII -> i III VI
};

static inline uint song_rand(uint* state) {
    *state = *state * 1103515245u + 12345u;
    return (*state >> 16) & 0x7fff;
}

// a scale degree (any octave) to a note index, from the tonic
static inline int song_degree_note(const Song* song, int degree) {
    int octave = 0;
    while (degree < 0) {
        degree += 7;
        octave--;
    }
    octave += degree / 7;
    degree %= 7;
    return song->tonic + octave * 12 + song_modes[song->mode][degree];
}

// is the degree in the chord (its root, third or fifth)
static inline byte song_is_chord_tone(int degree, byte chord) {
    int d = ((degree - chord) % 7 + 7) % 7;
    return d == 0 || d == 2 || d == 4;
}

// the nearest chord tone to a degree
static inline int song_snap_to_chord(int degree, byte chord) {
    for (int offset = 0; offset < 4; offset++) {
        if (song_is_chord_tone(degree - offset, chord)) {
            return degree - offset;
        }
        if (song_is_chord_tone(degree + offset, chord)) {
            return degree + offset;
        }
    }
    return degree;
}

// a motif (room template): a two bar rhythm and a mostly stepwise contour
void compose_motif(uint* rng, SongMotif* motif, byte busy) {
    memset(motif, 0, sizeof(SongMotif));
    int step = 0;
    int degree = (int) (song_rand(rng) % 3) * 2;    // root, third or fifth
    while (step < song_motif_steps) {
        // longer notes on strong beats, the last one held
        static const byte lengths[] = { 1, 2, 2, 3, 4 };
        byte length = lengths[song_rand(rng) % (busy ? 3 : 5)];
        if (step % 4 == 0 && song_rand(rng) % 3 == 0) {
            length = 4;
        }
        if (step + length > song_motif_steps) {
            length = song_motif_steps - step;
        }
        // rests now and then, never on the first step
        const byte rest = step > 0 && song_rand(rng) % 7 == 0;
        if (!rest) {
            motif->onset[step] = 1;
            motif->degree[step] = degree;
            motif->length[step] = length;
        }
        step += length;
        // mostly steps, sometimes a leap, pulled back to the middle
        static const sbyte moves[] = { -1, 1, -1, 1, -2, 2, 0, -3, 3 };
        degree += moves[song_rand(rng) % 9];
        if (degree > 7) {
            degree -= 2;
        } else if (degree < -3) {
            degree += 2;
        }
    }
}

// walks the chord graph for a section, ending on a cadence
static inline void compose_chords(uint* rng, Song* song, const SongSection* section, byte last) {
    byte chord = 0;
    for (int bar = 0; bar < section->bars; bar++) {
        const int index = section->start_bar + bar;
        if (bar == 0) {
            // contrast and climax sections start away from home
            chord = section->role == song_role_contrast ? 5 : section->role == song_role_climax ? 3 : 0;
        } else if (bar == section->bars - 1) {
            // cadence: home at the end of the song, otherwise the dominant leads on
            chord = last ? 0 : (song_rand(rng) % 2 ? 4 : 6);
        } else {
            const byte* next = song_chord_next[chord];
            byte options = 0;
            while (options < 4 && (options == 0 || next[options])) {
                options++;
            }
            chord = next[song_rand(rng) % options];
        }
        song->chords[index] = chord;
    }
}

// fills a note if the step's voice is free
static inline void song_set_note(Song* song, int step, byte voice, int note, byte instrument, byte steps, float volume) {
    if (step < 0 || step >= (int) song->steps_count || note <= 0 || note >= 108) {
        return;
    }
    song->steps[step].voices[voice] = (SongNote) { (byte) note, instrument, steps, volume };
}

void compose_song(uint seed, byte lead_instrument, Song* song) {
    memset(song, 0, sizeof(Song));
    uint rng = seed ? seed : 1;
    // key and mode
    int weight_total = 0;
    for (int i = 0; i < song_modes_count; i++) {
        weight_total += song_mode_weights[i];
    }
    int pick = song_rand(&rng) % weight_total;
    for (int i = 0; i < song_modes_count; i++) {
        pick -= song_mode_weights[i];
        if (pick < 0) {
            song->mode = i;
            break;
        }
    }
    song->tonic = 52 + song_rand(&rng) % 8;     // E4 to B4
    song->step_seconds = 0.26f + 0.12f * (song_rand(&rng) % 100) / 100.0f;
    // the ensemble, the contrast lead always differs from the theme's
    static const byte contrast_leads[] = { instrument_harp, instrument_music_box, instrument_cello, instrument_choir };
    static const byte basses[] = { instrument_cello, instrument_cello, instrument_soft_piano };
    static const byte arpeggios[] = { instrument_harp, instrument_music_box, instrument_soft_piano };
    static const byte pads[] = { instrument_choir, instrument_choir, instrument_cello };
    song->theme_lead = lead_instrument;
    do {
        song->contrast_lead = contrast_leads[song_rand(&rng) % 4];
    } while (song->contrast_lead == song->theme_lead);
    song->bass = basses[song_rand(&rng) % 3];
    song->arpeggio = arpeggios[song_rand(&rng) % 3];
    song->pad = pads[song_rand(&rng) % 3];
    // floor plan
    const SongFormSection* form = song_forms[song_rand(&rng) % song_forms_count];
    int bar = 0;
    for (int i = 0; i < song_max_sections && form[i].role != 255; i++) {
        if ((bar + form[i].bars) * song_steps_per_bar > song_max_steps) {
            break;
        }
        song->sections[i] = (SongSection) { form[i].role, form[i].motif, form[i].variation, form[i].bars, (byte) bar };
        bar += form[i].bars;
        song->sections_count++;
    }
    song->steps_count = bar * song_steps_per_bar;
    // room templates, contrast and climax motifs are busier
    for (int m = 0; m < song_max_motifs; m++) {
        compose_motif(&rng, &song->motifs[m], m > 0);
    }
    // corridors
    for (int s = 0; s < song->sections_count; s++) {
        compose_chords(&rng, song, &song->sections[s], s == song->sections_count - 1);
    }
    // decorate each room
    for (int s = 0; s < song->sections_count; s++) {
        const SongSection* section = &song->sections[s];
        const SongMotif* motif = &song->motifs[section->motif];
        const byte role = section->role;
        // as loud as the menu's imported song (its piano notes peak near 0.75)
        const float loudness =
            role == song_role_climax ? 1.0f :
            role == song_role_contrast ? 0.95f :
            role == song_role_intro || role == song_role_outro ? 0.75f : 0.9f;
        // contrast sections and varied themes change instrument (the melody's register suits a cello too)
        const byte lead = role == song_role_contrast || section->variation == 1 ? song->contrast_lead : song->theme_lead;
        // the variation shifts the motif's second half, or thins it (outro)
        const int shift = section->variation == 1 ? 1 + (int) (song_rand(&rng) % 2) : 0;
        for (int b = 0; b < section->bars; b++) {
            const int bar_index = section->start_bar + b;
            const byte chord = song->chords[bar_index];
            const int bar_step = bar_index * song_steps_per_bar;
            const byte is_last_bar = b == section->bars - 1;
            // melody, not in the intro
            if (role != song_role_intro) {
                for (int i = 0; i < song_steps_per_bar; i++) {
                    const int motif_step = (b % 2) * song_steps_per_bar + i;
                    if (!motif->onset[motif_step]) {
                        continue;
                    }
                    // the outro keeps only its strong beats
                    if (section->variation == 2 && i % 4 != 0) {
                        continue;
                    }
                    int degree = motif->degree[motif_step] + (b >= 2 ? shift : 0);
                    // strong beats sit on the chord
                    if (i % 4 == 0) {
                        degree = song_snap_to_chord(degree, chord);
                    }
                    byte steps = motif->length[motif_step];
                    if (i + steps > song_steps_per_bar) {
                        steps = song_steps_per_bar - i;
                    }
                    song_set_note(song, bar_step + i, song_voice_melody, song_degree_note(song, degree),
                        lead, steps, loudness * (i % 4 == 0 ? 1.0f : 0.85f));
                }
            }
            // bass: the chord's root held, the fifth halfway in busier sections
            const int bass = song_degree_note(song, chord) - 24;
            const byte busy = role == song_role_contrast || role == song_role_climax;
            song_set_note(song, bar_step, song_voice_bass, bass, song->bass, busy ? 4 : 8, 0.65f * loudness);
            if (busy) {
                song_set_note(song, bar_step + 4, song_voice_bass, song_degree_note(song, chord + 4) - 24,
                    song->bass, 4, 0.6f * loudness);
            }
            // arpeggio: music box in the intro, the ensemble's in the contrast and climax
            if (role == song_role_intro || busy) {
                const byte instrument = role == song_role_intro ? instrument_music_box : song->arpeggio;
                static const byte pattern[] = { 0, 2, 4, 7, 4, 2, 0, 2 };
                for (int i = 0; i < song_steps_per_bar; i += 1 + (role == song_role_intro)) {
                    song_set_note(song, bar_step + i, song_voice_arpeggio,
                        song_degree_note(song, chord + pattern[i]) - 12, instrument, 2, 0.5f * loudness);
                }
            }
            // pad: holding the chord's third in the climax, a deep bell rings it in
            if (role == song_role_climax) {
                if (b == 0) {
                    song_set_note(song, bar_step, song_voice_pad, song_degree_note(song, chord) - 24,
                        instrument_bell, 8, 0.6f * loudness);
                } else {
                    song_set_note(song, bar_step, song_voice_pad, song_degree_note(song, chord + 2) - 12,
                        song->pad, 8, 0.45f * loudness);
                }
            }
            // the song ends home, the tonic held over the last bar
            if (s == song->sections_count - 1 && is_last_bar) {
                for (int i = 0; i < song_steps_per_bar; i++) {
                    song->steps[bar_step + i].voices[song_voice_melody].note = 0;
                }
                song_set_note(song, bar_step, song_voice_melody, song->tonic, song->theme_lead, 8, 0.9f * loudness);
            }
        }
    }
}
