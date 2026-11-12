// tst: music_file, music .zox files load, and save and load back the same
//  - the shipped res/music/music.zox loads its notes, each a valid length and pitch
//  - notes saved then loaded come back the same (a uint32 count, it was saved as an int
//      but loaded as a size_t, so the music never loaded)
//  - a file claiming more than MAX_NOTES notes is clamped (it used to overflow the notes array)
//  - run: bash bsh/tst.sh music_file
byte zox_tst_music_file_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_music_file_check(ecs* world) {
    (void) world;
    static MidiNote notes[MAX_NOTES];
    char* path = concat_file_path(resources_path, music_file_path);
    const int count = load_notes_from_file(notes, path);
    free(path);
    zox_log("[tst] res music has [%i] notes", count);
    zox_tst_check(count > 0, "res/music/music.zox loaded no notes");
    int invalid = 0;
    for (int i = 0; i < count; i++) {
        invalid += !(notes[i].length > 0 && notes[i].length < 2 && notes[i].frequency > 0);
    }
    zox_tst_check(invalid == 0, "[%i] of [%i] notes have an invalid length or pitch", invalid, count);
    // round trip
    const char* tmp_path = "../tst/music_roundtrip.zox";
    MidiNote saved[3] = {
        { .time = 0, .length = 0.5, .frequency = 220 },
        { .time = 0.5, .length = 0.25, .frequency = 330 },
        { .time = 0.75, .length = 1, .frequency = 440 },
    };
    zox_tst_check(save_notes_to_file(saved, 3, tmp_path) == 0, "couldn't save [%s]", tmp_path);
    static MidiNote loaded[MAX_NOTES];
    const int loaded_count = load_notes_from_file(loaded, tmp_path);
    zox_tst_check(loaded_count == 3, "saved [3] notes, loaded [%i]", loaded_count);
    zox_tst_check(loaded_count == 3 && !memcmp(saved, loaded, sizeof(saved)), "loaded notes differ from the saved ones");
    // a count past MAX_NOTES is clamped
    FILE* file = fopen(tmp_path, "wb");
    if (file) {
        const uint32_t too_many = MAX_NOTES * 4;
        fwrite(&too_many, sizeof(uint32_t), 1, file);
        static MidiNote filler[MAX_NOTES * 4];
        fwrite(filler, sizeof(MidiNote), MAX_NOTES * 4, file);
        fclose(file);
        const int clamped = load_notes_from_file(loaded, tmp_path);
        zox_tst_check(clamped == MAX_NOTES, "a file of [%i] notes loaded [%i], expected [%i]", MAX_NOTES * 4, clamped, MAX_NOTES);
    }
    remove(tmp_path);
}

// tst: music_plays, the main menu's music plays: it steps through its notes, spawning sounds
//  - turns sounds back on (--background turns them off for tests)
//  - run: bash bsh/tst.sh music_plays
int zox_tst_music_plays_frames = 0;
int zox_tst_music_plays_start_sounds = 0;
uint zox_tst_music_plays_start_note = 0;
double zox_tst_music_plays_start = 0;
entity zox_tst_music_plays_music = 0;

byte zox_tst_music_plays_ready(ecs* world) {
    zox_tst_music_plays_frames++;
    // tests run --background, which turns sounds off, this one turns them on (no audio device plays them)
    nosounds = 0;
    zox_tst_waiting = "the menu music to start";
    const entity game = local_game;
    const entity music = zox_valid(game) ? zox_get_link(world, game, MenuMusicLink) : 0;
    if (!zox_valid(music) || !zox_getv(music, MusicEnabled)) {
        return 0;
    }
    if (!zox_tst_music_plays_music) {
        zox_tst_music_plays_music = music;
        zox_tst_music_plays_start = zox_current_time;
        zox_tst_music_plays_start_note = zox_getv(music, MusicNote);
        zox_tst_music_plays_start_sounds = zox_count_types(Sound);
    }
    zox_tst_waiting = "a few seconds of music";
    return zox_current_time - zox_tst_music_plays_start > 3.0;
}

void zox_tst_music_plays_check(ecs* world) {
    const entity music = zox_tst_music_plays_music;
    const uint note = zox_getv(music, MusicNote);
    const int sounds = zox_count_types(Sound);
    zox_log("[tst] menu music at note [%i] (from [%i]), sounds [%i] (from [%i])",
        note, zox_tst_music_plays_start_note, sounds, zox_tst_music_plays_start_sounds);
    zox_tst_check(note != zox_tst_music_plays_start_note, "the menu music didn't move on from note [%i] in 3 seconds", note);
    zox_tst_check(zox_get_children_count(world, music) > 0, "the menu music has no notes");
    zox_tst_check(sounds > zox_tst_music_plays_start_sounds, "the menu music spawned no sounds");
}

// tst: music_timing, notes play when the playhead reaches their start, together when they start together
//  - a music of: a note at 0.2s, a three note chord at 0.6s, a note at 1.0s, looping at 1.4s
//      (high notes nothing else plays, so their sounds are told apart from the menu music)
//  - each note's sound appears once the playhead passes its start, not before
//  - the chord's notes appear on the same frame, the first note again after the loop
//  - run: bash bsh/tst.sh music_timing
#define zox_tst_timing_notes 5
const int zox_tst_timing_note_index[zox_tst_timing_notes] = { 90, 91, 92, 93, 94 };
const double zox_tst_timing_starts[zox_tst_timing_notes] = { 0.2, 0.6, 0.6, 0.6, 1.0 };
entity zox_tst_timing_music = 0;
int zox_tst_timing_frame = 0;
int zox_tst_timing_heard_frame[zox_tst_timing_notes];
double zox_tst_timing_heard_at[zox_tst_timing_notes];
int zox_tst_timing_first_count = 0;
byte zox_tst_timing_looped = 0;

// how many sounds play a note
static inline int zox_tst_timing_count(ecs* world, int note) {
    const float frequency = note_frequencies[note];
    int count = 0;
    ecs_iter_t it = ecs_each_id(world, ecs_id(SoundFrequency));
    while (ecs_each_next(&it)) {
        const SoundFrequency* frequencies = ecs_field(&it, SoundFrequency, 0);
        for (int i = 0; i < it.count; i++) {
            count += zox_has(it.entities[i], Sound) && fabsf(frequencies[i].value - frequency) < 0.01f;
        }
    }
    return count;
}

byte zox_tst_music_timing_ready(ecs* world) {
    // tests run --background, which turns sounds off, music only plays with them on
    nosounds = 0;
    zox_tst_timing_frame++;
    if (!zox_tst_timing_music) {
        if (!zox_valid(local_game)) {
            zox_tst_waiting = "the game";
            return 0;
        }
        const entity music = spawn_music(world, prefab_music, 1);
        zox_set(music, MusicLength, { 1.4 });
        for (int i = 0; i < zox_tst_timing_notes; i++) {
            spawn_note(world, prefab_note, music, zox_tst_timing_note_index[i], instrument_harp, zox_tst_timing_starts[i], 0.1f, 0.5f);
            zox_tst_timing_heard_frame[i] = -1;
        }
        zox_set(music, MusicEnabled, { 1 });
        zox_tst_timing_music = music;
        return 0;
    }
    zox_tst_waiting = "the music to play its notes and loop";
    const double playhead = zox_getv(zox_tst_timing_music, MusicTime);
    for (int i = 0; i < zox_tst_timing_notes; i++) {
        if (zox_tst_timing_heard_frame[i] < 0 && zox_tst_timing_count(world, zox_tst_timing_note_index[i]) > 0) {
            zox_tst_timing_heard_frame[i] = zox_tst_timing_frame;
            zox_tst_timing_heard_at[i] = playhead;
        }
    }
    // looped: the playhead went back to the start, the first note plays again
    if (zox_tst_timing_heard_frame[4] >= 0 && playhead < 0.6) {
        zox_tst_timing_looped = 1;
    }
    zox_tst_timing_first_count = zox_tst_timing_count(world, zox_tst_timing_note_index[0]);
    return zox_tst_timing_looped && zox_tst_timing_first_count >= 2;
}

void zox_tst_music_timing_check(ecs* world) {
    for (int i = 0; i < zox_tst_timing_notes; i++) {
        zox_log("[tst] note [%i] (start [%.1fs]) heard on frame [%i], the playhead at [%.2fs]",
            i, zox_tst_timing_starts[i], zox_tst_timing_heard_frame[i], zox_tst_timing_heard_at[i]);
        zox_tst_check(zox_tst_timing_heard_frame[i] >= 0, "note [%i] never played", i);
        // spawns are seen the frame after, a slow frame later still
        zox_tst_check(zox_tst_timing_heard_at[i] >= zox_tst_timing_starts[i] &&
            zox_tst_timing_heard_at[i] < zox_tst_timing_starts[i] + 0.35,
            "note [%i] (start [%.1fs]) played with the playhead at [%.2fs]", i, zox_tst_timing_starts[i], zox_tst_timing_heard_at[i]);
    }
    zox_tst_check(zox_tst_timing_heard_frame[1] == zox_tst_timing_heard_frame[2] && zox_tst_timing_heard_frame[2] == zox_tst_timing_heard_frame[3],
        "the chord's notes played on frames [%i, %i, %i]", zox_tst_timing_heard_frame[1], zox_tst_timing_heard_frame[2], zox_tst_timing_heard_frame[3]);
    zox_tst_check(zox_tst_timing_heard_frame[0] < zox_tst_timing_heard_frame[1] && zox_tst_timing_heard_frame[1] < zox_tst_timing_heard_frame[4],
        "the notes played out of order");
    zox_tst_check(zox_tst_timing_first_count >= 2, "the music didn't loop back to its first note");
    zox_delete(zox_tst_timing_music);
}

// tst: music_compose, songs are composed like dungeons: a floor plan of sections (rooms) reusing
// motifs (room templates), joined by chord progressions (corridors), decorated with layers
//  - for 20 seeds: enough sections, every melody and bass note in the song's key, the bass under
//      the melody, sections sharing a motif share its rhythm, mostly stepwise, ending on the tonic
//  - the same seed composes the same song, different seeds differ, and they vary in mode and form
//  - its performance: timed notes in order, in the key and in note_frequencies, chords (3 notes together),
//      varied lengths, some seeds swing, grace notes
//  - renders a song to bin/tst/song.wav, to listen to
//  - run: bash bsh/tst.sh music_compose
// mixes a song's notes (its performed events) as the music system plays them, each scaled by gain (the music volume)
float* zox_tst_mix_song(const Song* song, float gain, uint* out_length) {
    const uint length = (uint) ((song->seconds + 3) * sound_sample_rate);
    float* mix = calloc(length, sizeof(float));
    float* note = malloc(MAX_SOUND_DATA * sizeof(float));
    for (uint i = 0; i < song->events_count; i++) {
        const SongEvent event = song->events[i];
        const uint note_length = (uint) (event.seconds * sound_sample_rate);
        generate_instrument_samples(event.instrument, note_frequencies[event.note], event.seconds, event.volume * gain, 0, note, note_length);
        const uint start = (uint) (event.start * sound_sample_rate);
        for (uint j = 0; j < note_length && start + j < length; j++) {
            mix[start + j] += note[j];
        }
    }
    free(note);
    *out_length = length;
    return mix;
}

// renders a song into a .wav to listen to, normalized
void zox_tst_render_song(const Song* song, const char* path) {
    uint length;
    float* mix = zox_tst_mix_song(song, 1, &length);
    float peak = 0;
    for (uint j = 0; j < length; j++) {
        peak = fmaxf(peak, fabsf(mix[j]));
    }
    if (peak > 0.9f) {
        for (uint j = 0; j < length; j++) {
            mix[j] *= 0.9f / peak;
        }
    }
    zox_tst_write_wav(path, mix, length);
    free(mix);
}

byte zox_tst_music_compose_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_music_compose_check(ecs* world) {
    (void) world;
    static Song song;
    static Song again;
    byte modes_seen[song_modes_count] = { 0 };
    byte section_counts_seen[song_max_sections + 1] = { 0 };
    byte contrast_leads_seen[instrument_end] = { 0 };
    int swings = 0, graces = 0;
    for (uint seed = 1; seed <= 20; seed++) {
        compose_song(seed, instrument_soft_piano, &song);
        modes_seen[song.mode] = 1;
        section_counts_seen[song.sections_count] = 1;
        zox_tst_check(song.sections_count >= 5, "seed [%u] has [%i] sections", seed, song.sections_count);
        zox_tst_check(song.steps_count >= 240 && song.steps_count <= song_max_steps, "seed [%u] has [%u] steps", seed, song.steps_count);
        zox_tst_check(song.steps_count * song.step_seconds >= 70, "seed [%u] lasts only [%.0fs]", seed, song.steps_count * song.step_seconds);
        // an ensemble: the lead changes for contrast, and other voices bring their own instruments
        byte instruments_seen[instrument_end] = { 0 };
        int instruments = 0;
        for (uint step = 0; step < song.steps_count; step++) {
            for (int v = 0; v < song_voices; v++) {
                const SongNote note = song.steps[step].voices[v];
                if (note.note && note.instrument < instrument_end && !instruments_seen[note.instrument]) {
                    instruments_seen[note.instrument] = 1;
                    instruments++;
                }
            }
        }
        zox_tst_check(instruments >= 4, "seed [%u] only plays [%i] instruments", seed, instruments);
        zox_tst_check(song.contrast_lead != song.theme_lead, "seed [%u] contrasts with its theme's instrument", seed);
        contrast_leads_seen[song.contrast_lead] = 1;
        int notes = 0, out_of_key = 0, bass_above = 0, leaps = 0, last_melody = 0, previous = 0;
        for (uint step = 0; step < song.steps_count; step++) {
            const SongNote melody = song.steps[step].voices[song_voice_melody];
            const SongNote bass = song.steps[step].voices[song_voice_bass];
            if (melody.note) {
                notes++;
                out_of_key += !song_in_key(&song, melody.note);
                if (previous && abs(melody.note - previous) > 7) {
                    leaps++;
                }
                previous = melody.note;
                last_melody = melody.note;
            }
            if (bass.note) {
                out_of_key += !song_in_key(&song, bass.note);
                bass_above += melody.note && bass.note >= melody.note;
            }
        }
        zox_tst_check(out_of_key == 0, "seed [%u] has [%i] notes out of its key", seed, out_of_key);
        zox_tst_check(bass_above == 0, "seed [%u] has its bass above its melody [%i] times", seed, bass_above);
        zox_tst_check(leaps * 100 <= notes * 15, "seed [%u] leaps [%i] of [%i] notes", seed, leaps, notes);
        zox_tst_check(last_melody == song.tonic, "seed [%u] ends on [%i], not its tonic [%i]", seed, last_melody, song.tonic);
        // sections sharing a motif (unvaried) share its rhythm, over their first two bars
        for (int a = 0; a < song.sections_count; a++) {
            for (int b = a + 1; b < song.sections_count; b++) {
                const SongSection* sa = &song.sections[a];
                const SongSection* sb = &song.sections[b];
                if (sa->motif != sb->motif || sa->variation || sb->variation || sa->role == song_role_intro || sb->role == song_role_intro) {
                    continue;
                }
                int differ = 0;
                for (int i = 0; i < song_motif_steps; i++) {
                    const byte on_a = song.steps[sa->start_bar * song_steps_per_bar + i].voices[song_voice_melody].note != 0;
                    const byte on_b = song.steps[sb->start_bar * song_steps_per_bar + i].voices[song_voice_melody].note != 0;
                    differ += on_a != on_b;
                }
                zox_tst_check(differ == 0, "seed [%u] sections [%i] and [%i] share a motif but not its rhythm", seed, a, b);
            }
        }
        // the performance: timed notes, overlapping freely
        int unsorted = 0, outside = 0, performed_out_of_key = 0, chords = 0;
        float lengths_seen[64];
        int lengths_count = 0;
        for (uint e = 0; e < song.events_count; e++) {
            const SongEvent event = song.events[e];
            unsorted += e > 0 && event.start < song.events[e - 1].start;
            outside += event.start < 0 || event.start > song.seconds || event.seconds <= 0 || event.seconds > song_max_note_seconds;
            performed_out_of_key += !song_in_key(&song, event.note);
            outside += event.note >= notes_max;
            graces += event.voice == song_voice_melody && event.seconds < 0.1f;
            // a chord: three or more notes starting within 70ms
            int together = 0;
            for (uint f = e; f < song.events_count && song.events[f].start - event.start < 0.07f; f++) {
                together++;
            }
            chords += together >= 3;
            byte known = 0;
            for (int l = 0; l < lengths_count; l++) {
                known |= fabsf(lengths_seen[l] - event.seconds) < 0.01f;
            }
            if (!known && lengths_count < 64) {
                lengths_seen[lengths_count++] = event.seconds;
            }
        }
        swings += song.swing;
        zox_tst_check(song.events_count > notes, "seed [%u] performed [%u] notes from [%i] melody notes", seed, song.events_count, notes);
        zox_tst_check(unsorted == 0, "seed [%u] has [%i] notes out of order", seed, unsorted);
        zox_tst_check(outside == 0, "seed [%u] has [%i] notes outside the song or too long", seed, outside);
        zox_tst_check(performed_out_of_key == 0, "seed [%u] performed [%i] notes out of its key", seed, performed_out_of_key);
        zox_tst_check(chords > 0, "seed [%u] plays no chords (3 notes together)", seed);
        zox_tst_check(lengths_count >= 5, "seed [%u] only has [%i] note lengths", seed, lengths_count);
        compose_song(seed, instrument_soft_piano, &again);
        zox_tst_check(!memcmp(&song, &again, sizeof(Song)), "seed [%u] composed a different song the second time", seed);
    }
    compose_song(1, instrument_soft_piano, &song);
    compose_song(2, instrument_soft_piano, &again);
    zox_tst_check(memcmp(&song, &again, sizeof(Song)), "seeds [1] and [2] composed the same song");
    int modes = 0, forms = 0;
    for (int i = 0; i < song_modes_count; i++) {
        modes += modes_seen[i];
    }
    for (int i = 0; i <= song_max_sections; i++) {
        forms += section_counts_seen[i];
    }
    zox_tst_check(modes >= 2, "20 seeds only used [%i] modes", modes);
    zox_tst_check(forms >= 2, "20 seeds only used [%i] floor plans", forms);
    int contrast_leads = 0;
    for (int i = 0; i < instrument_end; i++) {
        contrast_leads += contrast_leads_seen[i];
    }
    zox_tst_check(contrast_leads >= 3, "20 seeds only used [%i] contrast leads", contrast_leads);
    zox_tst_check(swings > 0 && swings < 20, "[%i] of 20 seeds swing, some should and some not", swings);
    zox_tst_check(graces > 0, "20 seeds played no grace notes");
    // listen to one
    compose_song(666, instrument_soft_piano, &song);
    zox_log("[tst] song [666]: [%i] sections, [%u] steps of [%.2fs], [%u] notes over [%.0fs], mode [%i], tonic [%i], swing [%i]",
        song.sections_count, song.steps_count, song.step_seconds, song.events_count, song.seconds, song.mode, song.tonic, song.swing);
    zox_tst_render_song(&song, "../tst/song.wav");
}

// tst: sound_balance, sound effects stand out over the music, at the game's volumes, on small speakers
//  - mixes the realm's song (seed 666) at the music volume, as the game plays it
//  - drops every effect (sound_effects in sounds/fun/effects.c) at its middle values every few seconds
//  - each must be louder than the music under it (rms over its length, by sound_balance_ratio, under _max),
//      heard through a small speaker (a high pass at sound_balance_speaker_hz, laptops barely play below)
//  - writes bin/tst/sound_balance.wav, the song with the effects, to listen to
//  - run: bash bsh/tst.sh sound_balance
#define sound_balance_ratio 2.0f            // about 6db over the music
#define sound_balance_ratio_max 8.0f        // and not painfully over it
#define sound_balance_speaker_hz 200.0f

// a second order high pass (butterworth), like a small speaker's bass roll off
static inline void zox_tst_speaker(const float* in, float* out, uint length) {
    const float w = 2 * M_PI * sound_balance_speaker_hz / sound_sample_rate;
    const float alpha = sinf(w) / (2 * 0.7071f);
    const float cw = cosf(w);
    const float a0 = 1 + alpha;
    const float b0 = (1 + cw) / 2 / a0, b1 = -(1 + cw) / a0, b2 = (1 + cw) / 2 / a0;
    const float a1 = -2 * cw / a0, a2 = (1 - alpha) / a0;
    float x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    for (uint j = 0; j < length; j++) {
        const float x = in[j];
        const float y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        x2 = x1; x1 = x; y2 = y1; y1 = y;
        out[j] = y;
    }
}

static inline double zox_tst_power(const float* samples, uint length) {
    double power = 0;
    for (uint j = 0; j < length; j++) {
        power += samples[j] * samples[j];
    }
    return power;
}

byte zox_tst_sound_balance_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_sound_balance_check(ecs* world) {
    (void) world;
    static Song song;
    compose_song(666, instrument_soft_piano, &song);
    uint length;
    float* mix = zox_tst_mix_song(&song, get_volume_music(), &length);
    float* heard_music = malloc(length * sizeof(float));
    zox_tst_speaker(mix, heard_music, length);
    float* sfx = malloc(MAX_SOUND_DATA * sizeof(float));
    float* heard_sfx = malloc(MAX_SOUND_DATA * sizeof(float));
    float worst[sound_effect_end];
    for (int i = 0; i < sound_effect_end; i++) {
        worst[i] = 1000;
    }
    const uint gap = 3 * sound_sample_rate / 2;
    int placed = 0;
    for (uint start = sound_sample_rate; start + MAX_SOUND_DATA < length; start += gap, placed++) {
        const int type = placed % sound_effect_end;
        const SoundEffect effect = sound_effects[type];
        const float seconds = (effect.seconds_min + effect.seconds_max) / 2;
        const float volume = (effect.volume_min + effect.volume_max) / 2;
        const byte note = (effect.note_min + effect.note_max) / 2;
        const uint sfx_length = (uint) (seconds * sound_sample_rate);
        generate_instrument_samples(effect.instrument, note_frequencies[note], seconds, volume * get_volume_sfx(), 0, sfx, sfx_length);
        zox_tst_speaker(sfx, heard_sfx, sfx_length);
        const double music_power = zox_tst_power(heard_music + start, sfx_length);
        const double sfx_power = zox_tst_power(heard_sfx, sfx_length);
        const float ratio = music_power > 0 ? sqrtf(sfx_power / music_power) : 1000;
        worst[type] = fminf(worst[type], ratio);
        for (uint j = 0; j < sfx_length; j++) {
            mix[start + j] += sfx[j];
        }
    }
    for (int i = 0; i < sound_effect_end; i++) {
        zox_log("[tst] [%s] at least [%.2f]x the music", sound_effects[i].name, worst[i]);
        zox_tst_check(worst[i] >= sound_balance_ratio, "[%s] is only [%.2f]x the music under it, on small speakers",
            sound_effects[i].name, worst[i]);
        zox_tst_check(worst[i] <= sound_balance_ratio_max, "[%s] is [%.2f]x the music, too loud", sound_effects[i].name, worst[i]);
    }
    zox_tst_check(placed >= 3 * sound_effect_end, "only [%i] effects placed", placed);
    zox_tst_write_wav("../tst/sound_balance.wav", mix, length);
    free(heard_sfx);
    free(sfx);
    free(heard_music);
    free(mix);
}

void zox_tsts_musics() {
    zox_tst_add("music_timing", zox_tst_music_timing_ready, zox_tst_music_timing_check, 30);
    zox_tst_set_setup("music_timing", NULL, 1);
    zox_tst_add("sound_balance", zox_tst_sound_balance_ready, zox_tst_sound_balance_check, 60);
    zox_tst_set_setup("sound_balance", NULL, 1);
    zox_tst_add("music_compose", zox_tst_music_compose_ready, zox_tst_music_compose_check, 60);
    zox_tst_set_setup("music_compose", NULL, 1);
    zox_tst_add("music_file", zox_tst_music_file_ready, zox_tst_music_file_check, 30);
    zox_tst_set_setup("music_file", NULL, 1);
    zox_tst_add("music_plays", zox_tst_music_plays_ready, zox_tst_music_plays_check, 60);
    zox_tst_set_setup("music_plays", NULL, 1);
}
