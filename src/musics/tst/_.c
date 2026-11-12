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

// tst: music_compose, songs are composed like dungeons: a floor plan of sections (rooms) reusing
// motifs (room templates), joined by chord progressions (corridors), decorated with layers
//  - for 20 seeds: enough sections, every melody and bass note in the song's key, the bass under
//      the melody, sections sharing a motif share its rhythm, mostly stepwise, ending on the tonic
//  - the same seed composes the same song, different seeds differ, and they vary in mode and form
//  - renders a song to bin/tst/song.wav, to listen to
//  - run: bash bsh/tst.sh music_compose
static inline byte zox_tst_song_in_key(const Song* song, int note) {
    const int semitone = ((note - song->tonic) % 12 + 12) % 12;
    for (int d = 0; d < 7; d++) {
        if (song_modes[song->mode][d] == semitone) {
            return 1;
        }
    }
    return 0;
}

// mixes a song's notes as the music system plays them, each note scaled by gain (the game's music volume)
float* zox_tst_mix_song(const Song* song, float gain, uint* out_length) {
    const uint step_samples = (uint) (song->step_seconds * sound_sample_rate);
    const uint length = song->steps_count * step_samples + 3 * sound_sample_rate;
    float* mix = calloc(length, sizeof(float));
    float* note = malloc(MAX_SOUND_DATA * sizeof(float));
    for (uint step = 0; step < song->steps_count; step++) {
        for (byte v = 0; v < song_voices; v++) {
            const SongNote n = song->steps[step].voices[v];
            if (!n.note) {
                continue;
            }
            const float seconds = fminf(2.8f, n.steps * song->step_seconds * (v ? 1.1f : 1.0f));
            const uint note_length = (uint) (seconds * sound_sample_rate);
            generate_instrument_samples(n.instrument, note_frequencies[n.note], seconds, n.volume * gain, 0, note, note_length);
            const uint start = step * step_samples;
            for (uint j = 0; j < note_length && start + j < length; j++) {
                mix[start + j] += note[j];
            }
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
                out_of_key += !zox_tst_song_in_key(&song, melody.note);
                if (previous && abs(melody.note - previous) > 7) {
                    leaps++;
                }
                previous = melody.note;
                last_melody = melody.note;
            }
            if (bass.note) {
                out_of_key += !zox_tst_song_in_key(&song, bass.note);
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
    // listen to one
    compose_song(666, instrument_soft_piano, &song);
    zox_log("[tst] song [666]: [%i] sections, [%u] steps of [%.2fs], mode [%i], tonic [%i]",
        song.sections_count, song.steps_count, song.step_seconds, song.mode, song.tonic);
    zox_tst_render_song(&song, "../tst/song.wav");
}

// tst: sound_balance, sound effects stand out over the music, at the game's volumes
//  - mixes the realm's song (seed 666) at the music volume, as the game plays it
//  - drops the common effects (jump, land, hit, swing miss, consume) at their game volumes every few seconds
//  - each must be louder than the music under it (rms over its length, by sound_balance_ratio)
//  - writes bin/tst/sound_balance.wav, the song with the effects, to listen to
//  - run: bash bsh/tst.sh sound_balance
#define sound_balance_ratio 2.0f    // about 6db over the music
typedef struct {
    const char* name;
    byte instrument;
    byte note;
    float seconds;
    float volume;   // of the sfx volume
} ZoxTstSfx;

byte zox_tst_sound_balance_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_sound_balance_check(ecs* world) {
    (void) world;
    // as the game spawns them (jump_sound.c, land_sound.c, damage.c, melee.c, consume.c), at their middle values
    const ZoxTstSfx effects[] = {
        { "jump", instrument_edm, 29, 0.21f, 1.25f },
        { "land", instrument_edm, 19, 0.35f, 1.8f },
        { "hit", instrument_violin, 28, 0.6f, 1.0f },
        { "swing miss", instrument_violin, 44, 0.3f, 1.0f },
        { "consume", instrument_violin, 42, 0.4f, 1.2f },
    };
    const int effects_count = sizeof(effects) / sizeof(effects[0]);
    static Song song;
    compose_song(666, instrument_soft_piano, &song);
    uint length;
    float* mix = zox_tst_mix_song(&song, get_volume_music(), &length);
    float* sfx = malloc(MAX_SOUND_DATA * sizeof(float));
    float worst = 1000;
    const uint gap = 3 * sound_sample_rate;
    int placed = 0;
    for (uint start = sound_sample_rate; start + MAX_SOUND_DATA < length; start += gap, placed++) {
        const ZoxTstSfx effect = effects[placed % effects_count];
        const uint sfx_length = (uint) (effect.seconds * sound_sample_rate);
        generate_instrument_samples(effect.instrument, note_frequencies[effect.note], effect.seconds,
            effect.volume * get_volume_sfx(), 0, sfx, sfx_length);
        double sfx_power = 0, music_power = 0;
        for (uint j = 0; j < sfx_length; j++) {
            sfx_power += sfx[j] * sfx[j];
            music_power += mix[start + j] * mix[start + j];
            mix[start + j] += sfx[j];
        }
        const float ratio = music_power > 0 ? sqrtf(sfx_power / music_power) : 1000;
        worst = fminf(worst, ratio);
        zox_tst_check(ratio >= sound_balance_ratio, "[%s] at [%.1fs] is only [%.2f]x the music under it",
            effect.name, start / (float) sound_sample_rate, ratio);
    }
    zox_log("[tst] [%i] effects over the music, the quietest [%.2f]x louder", placed, worst);
    zox_tst_check(placed >= 20, "only [%i] effects placed", placed);
    zox_tst_write_wav("../tst/sound_balance.wav", mix, length);
    free(sfx);
    free(mix);
}

void zox_tsts_musics() {
    zox_tst_add("sound_balance", zox_tst_sound_balance_ready, zox_tst_sound_balance_check, 60);
    zox_tst_set_setup("sound_balance", NULL, 1);
    zox_tst_add("music_compose", zox_tst_music_compose_ready, zox_tst_music_compose_check, 60);
    zox_tst_set_setup("music_compose", NULL, 1);
    zox_tst_add("music_file", zox_tst_music_file_ready, zox_tst_music_file_check, 30);
    zox_tst_set_setup("music_file", NULL, 1);
    zox_tst_add("music_plays", zox_tst_music_plays_ready, zox_tst_music_plays_check, 60);
    zox_tst_set_setup("music_plays", NULL, 1);
}
