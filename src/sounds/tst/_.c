// tst: sound_formats, sounds are mono 44100 hz, the format every play path uses
//  - every loaded sound file is mono: its samples are its length in seconds * 44100
//      (stereo wavs are downmixed, they were kept interleaved and played as stereo, while
//      mono ones played as stereo too, at double speed)
//  - a generated sound's data is its length in seconds * 44100
//  - run: bash bsh/tst.sh sound_formats
int zox_tst_sound_frames = 0;
entity zox_tst_sound_generated = 0;

byte zox_tst_sound_formats_ready(ecs* world) {
    zox_tst_sound_frames++;
    if (zox_tst_sound_frames == 5) {
        zox_tst_sound_generated = spawn_sound_generated(world, prefab_sound_generated, instrument_piano, 440, 0.5f, 0.5f);
    }
    zox_tst_waiting = "a few frames";
    return zox_tst_sound_frames >= 10;
}

void zox_tst_sound_formats_check(ecs* world) {
    int checked = 0;
    for (int i = 0; i < sound_files_count; i++) {
        const entity e = files_sounds[i];
        if (!zox_valid(e) || !zox_has(e, SoundData) || !zox_has(e, SoundLength)) {
            continue;
        }
        const SoundData* data = zox_get(e, SoundData);
        const float seconds = zox_getv(e, SoundLength);
        const int expected = (int) (seconds * sound_sample_rate + 0.5f);
        // MAX_SOUND_DATA cuts long sounds
        const int expected_cut = expected < MAX_SOUND_DATA ? expected : MAX_SOUND_DATA;
        zox_tst_check(abs((int) data->length - expected_cut) <= 1,
            "sound file [%s] has [%u] samples for [%.3fs], expected [%i] (mono 44100)",
            zox_getn(e), data->length, seconds, expected_cut);
        checked++;
    }
    zox_log("[tst] checked [%i] sound files", checked);
    zox_tst_check(checked > 0, "no sound files loaded");
    zox_tst_check(zox_valid(zox_tst_sound_generated), "the generated sound wasn't spawned");
    if (zox_valid(zox_tst_sound_generated)) {
        const uint length = zox_get(zox_tst_sound_generated, SoundData)->length;
        zox_tst_check(length == (uint) (0.5f * sound_sample_rate), "the generated sound has [%u] samples for 0.5s", length);
    }
}


// tst: sound_instruments, every instrument makes a clean, audible note at its pitch
//  - generates half a second of A3 (220 hz) per instrument, checks:
//      - every sample is a number, the note is audible and doesn't clip (samples cut at full scale)
//      - it starts and ends near silence (no clicks)
//      - the pitched instruments sound at their pitch (autocorrelation), the music box an octave up
//  - writes a short phrase per instrument to bin/tst/instruments/<name>.wav, to listen to
//  - run: bash bsh/tst.sh sound_instruments
#define zox_tst_instrument_seconds 0.5f
#define zox_tst_instrument_frequency 220.0f

// the strongest period near the expected one, as a frequency (autocorrelation)
float zox_tst_instrument_pitch(const float* samples, uint length, float expected) {
    const uint start = (uint) (0.05f * sound_sample_rate);
    const uint window = 4096;
    if (start + window * 2 > length) {
        return 0;
    }
    const uint min_lag = (uint) (sound_sample_rate / (expected * 1.6f));
    const uint max_lag = (uint) (sound_sample_rate / (expected / 1.6f));
    float best = 0;
    uint best_lag = 0;
    for (uint lag = min_lag; lag <= max_lag; lag++) {
        float sum = 0;
        for (uint i = 0; i < window; i++) {
            sum += samples[start + i] * samples[start + i + lag];
        }
        if (sum > best) {
            best = sum;
            best_lag = lag;
        }
    }
    return best_lag ? sound_sample_rate / (float) best_lag : 0;
}

// writes mono float samples as a .wav, to listen to them
static inline void zox_tst_write_wav(const char* path, const float* samples, uint length) {
    FILE* file = fopen(path, "wb");
    if (!file) {
        return;
    }
    const uint32_t data_size = length * sizeof(float);
    const uint32_t riff_size = 36 + data_size;
    const uint32_t fmt_size = 16;
    const uint16_t format = 3;   // ieee float
    const uint16_t channels = 1;
    const uint32_t rate = sound_sample_rate;
    const uint32_t byte_rate = rate * sizeof(float);
    const uint16_t block_align = sizeof(float);
    const uint16_t bits = 32;
    fwrite("RIFF", 1, 4, file);
    fwrite(&riff_size, 4, 1, file);
    fwrite("WAVEfmt ", 1, 8, file);
    fwrite(&fmt_size, 4, 1, file);
    fwrite(&format, 2, 1, file);
    fwrite(&channels, 2, 1, file);
    fwrite(&rate, 4, 1, file);
    fwrite(&byte_rate, 4, 1, file);
    fwrite(&block_align, 2, 1, file);
    fwrite(&bits, 2, 1, file);
    fwrite("data", 1, 4, file);
    fwrite(&data_size, 4, 1, file);
    fwrite(samples, sizeof(float), length, file);
    fclose(file);
}

// a slow a minor arpeggio of the instrument, to bin/tst/instruments/<name>.wav
static inline void zox_tst_write_instrument_phrase(byte instrument) {
    static const float notes[] = { 220.0f, 261.63f, 329.63f, 440.0f };
    const float note_seconds = 0.9f;
    const uint note_length = (uint) (note_seconds * sound_sample_rate);
    const uint step = (uint) (0.45f * sound_sample_rate);
    const uint length = step * 3 + note_length;
    float* phrase = calloc(length, sizeof(float));
    float* note = malloc(note_length * sizeof(float));
    for (int i = 0; i < 4; i++) {
        generate_instrument_samples(instrument, notes[i], note_seconds, 0.45f, 0, note, note_length);
        for (uint j = 0; j < note_length; j++) {
            phrase[step * i + j] += note[j];
        }
    }
    char path[128];
    snprintf(path, sizeof(path), "../tst/instruments/%s.wav", instruments[instrument].name);
    zox_tst_write_wav(path, phrase, length);
    free(note);
    free(phrase);
}

byte zox_tst_sound_instruments_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_sound_instruments_check(ecs* world) {
    (void) world;
    const uint length = (uint) (zox_tst_instrument_seconds * sound_sample_rate);
    float* samples = malloc(length * sizeof(float));
    mkdir("../tst/instruments", 0755);
    for (byte instrument = instrument_none + 1; instrument < instrument_end; instrument++) {
        const char* name = instruments[instrument].name;
        zox_tst_check(generate_instrument_samples(instrument, zox_tst_instrument_frequency, zox_tst_instrument_seconds, 1, 0, samples, length),
            "instrument [%i] didn't generate", instrument);
        float peak = 0;
        int invalid = 0;
        int clipped = 0;
        for (uint j = 0; j < length; j++) {
            if (!isfinite(samples[j])) {
                invalid++;
                continue;
            }
            peak = fmaxf(peak, fabsf(samples[j]));
            clipped += fabsf(samples[j]) >= 0.999f;
        }
        zox_tst_check(invalid == 0, "[%s] has [%i] invalid samples", name, invalid);
        zox_tst_check(peak > 0.02f, "[%s] is silent, peak [%.3f]", name, peak);
        zox_tst_check(clipped == 0, "[%s] clips on [%i] samples", name, clipped);
        zox_tst_check(fabsf(samples[0]) < 0.05f, "[%s] clicks as it starts [%.3f]", name, samples[0]);
        zox_tst_check(fabsf(samples[length - 1]) < 0.05f, "[%s] clicks as it ends [%.3f]", name, samples[length - 1]);
        // pitch, for the instruments that play their note (the bell hums an octave below)
        float expected = zox_tst_instrument_frequency;
        if (instrument == instrument_music_box) {
            expected *= 2;
        }
        const byte is_pitched =
            instrument == instrument_piano ||
            instrument == instrument_organ ||
            instrument == instrument_soft_piano ||
            instrument == instrument_harp ||
            instrument == instrument_music_box ||
            instrument == instrument_cello ||
            instrument == instrument_choir;
        if (is_pitched) {
            const float pitch = zox_tst_instrument_pitch(samples, length, expected);
            zox_tst_check(fabsf(pitch - expected) / expected < 0.04f, "[%s] sounds at [%.1f] hz, expected [%.1f]", name, pitch, expected);
        }
        zox_log("[tst] [%s] peak [%.2f] clipped [%i]", name, peak, clipped);
        zox_tst_write_instrument_phrase(instrument);
    }
    free(samples);
}

// tst: sound_mixer, the engine's mixer and 3D sounds (no audio device, renders by hand)
//  - a sound without a position plays in the middle, both ears the same
//  - a 3D sound to the listener's right is louder on the right, turning around swaps them
//  - further is quieter, past sound_max_distance it doesn't play
//  - voices free when they finish, a full mixer makes way for a new sound, the limiter keeps it under 1
//  - run: bash bsh/tst.sh sound_mixer
#define zox_tst_mixer_length 1000
static float zox_tst_mixer_out[2 * zox_tst_mixer_length];

// plays a constant 0.5 sound, renders one frame of it, returns its left and right
static inline SoundGains zox_tst_mixer_hear(const float3* source) {
    static float samples[zox_tst_mixer_length];
    for (int j = 0; j < zox_tst_mixer_length; j++) {
        samples[j] = 0.5f;
    }
    zox_mixer_stop_all();
    if (zox_mixer_play(samples, zox_tst_mixer_length, source) < 0) {
        return (SoundGains) { 0, 0 };
    }
    zox_mixer_render(zox_tst_mixer_out, 1);
    return (SoundGains) { zox_tst_mixer_out[0], zox_tst_mixer_out[1] };
}

byte zox_tst_sound_mixer_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_sound_mixer_check(ecs* world) {
    (void) world;
    const byte listener_was = sound_listener_active;
    const float3 listener_position = sound_listener_position;
    const float4 listener_rotation = sound_listener_rotation;
    zox_mixer_set_listener(float3_zero, float4_identity);
    // in the middle
    SoundGains heard = zox_tst_mixer_hear(NULL);
    zox_tst_check(fabsf(heard.left - 0.5f) < 0.001f && fabsf(heard.right - 0.5f) < 0.001f,
        "a 2D sound plays [%.3f, %.3f], not [0.5, 0.5]", heard.left, heard.right);
    // right and left (forward is -z, right is +x)
    const float3 right = { 5, 0, 0 };
    const float3 left = { -5, 0, 0 };
    const float3 ahead = { 0, 0, -5 };
    heard = zox_tst_mixer_hear(&right);
    zox_tst_check(heard.right > heard.left * 2, "a sound on the right plays [%.3f, %.3f]", heard.left, heard.right);
    zox_tst_check(heard.left > 0, "a sound on the right is silent in the left ear");
    heard = zox_tst_mixer_hear(&left);
    zox_tst_check(heard.left > heard.right * 2, "a sound on the left plays [%.3f, %.3f]", heard.left, heard.right);
    heard = zox_tst_mixer_hear(&ahead);
    zox_tst_check(fabsf(heard.left - heard.right) < 0.001f, "a sound ahead plays [%.3f, %.3f]", heard.left, heard.right);
    // turned around, right is left
    zox_mixer_set_listener(float3_zero, quaternion_from_euler((float3) { 0, M_PI, 0 }));
    heard = zox_tst_mixer_hear(&right);
    zox_tst_check(heard.left > heard.right * 2, "turned around, a sound on +x plays [%.3f, %.3f]", heard.left, heard.right);
    zox_mixer_set_listener(float3_zero, float4_identity);
    // distance
    float last = 1000;
    byte quieter = 1;
    for (float distance = 1; distance < sound_max_distance; distance += 4) {
        const float3 source = { 0, 0, -distance };
        heard = zox_tst_mixer_hear(&source);
        const float loudness = heard.left + heard.right;
        quieter &= loudness <= last + 0.0001f;
        last = loudness;
    }
    zox_tst_check(quieter, "sounds don't get quieter with distance");
    const float3 near = { 0, 0, -sound_min_distance };
    heard = zox_tst_mixer_hear(&near);
    zox_tst_check(fabsf(heard.left - 0.5f) < 0.001f, "within sound_min_distance it plays [%.3f], not full", heard.left);
    const float3 far = { 0, 0, -sound_max_distance - 1 };
    zox_mixer_stop_all();
    static float samples[zox_tst_mixer_length];
    for (int j = 0; j < zox_tst_mixer_length; j++) {
        samples[j] = 0.5f;
    }
    zox_tst_check(zox_mixer_play(samples, zox_tst_mixer_length, &far) < 0, "a sound past max distance played");
    // a moving listener moves a playing sound
    zox_mixer_stop_all();
    zox_mixer_play(samples, zox_tst_mixer_length, &right);
    zox_mixer_set_listener(float3_zero, quaternion_from_euler((float3) { 0, M_PI, 0 }));
    zox_mixer_render(zox_tst_mixer_out, 1);
    zox_tst_check(zox_tst_mixer_out[0] > zox_tst_mixer_out[1], "a playing sound didn't follow the listener turning");
    zox_mixer_set_listener(float3_zero, float4_identity);
    // a delayed voice waits for its moment (a jingle's notes)
    zox_mixer_stop_all();
    zox_mixer_play_delayed(samples, zox_tst_mixer_length, NULL, 0.01f);   // 441 samples
    zox_mixer_render(zox_tst_mixer_out, 400);
    float early = 0;
    for (int j = 0; j < 800; j++) {
        early = fmaxf(early, fabsf(zox_tst_mixer_out[j]));
    }
    zox_tst_check(early == 0, "a delayed voice played [%.3f] before its delay", early);
    zox_mixer_render(zox_tst_mixer_out, 100);
    zox_tst_check(zox_tst_mixer_out[0] == 0 && fabsf(zox_tst_mixer_out[2 * 60] - 0.5f) < 0.001f,
        "a delayed voice didn't start on its sample [%.3f, %.3f]", zox_tst_mixer_out[0], zox_tst_mixer_out[2 * 60]);
    // voices finish
    zox_mixer_stop_all();
    zox_mixer_play(samples, zox_tst_mixer_length, NULL);
    zox_tst_check(zox_mixer_voices_playing() == 1, "[%i] voices playing, not 1", zox_mixer_voices_playing());
    zox_mixer_render(zox_tst_mixer_out, zox_tst_mixer_length);
    zox_tst_check(zox_mixer_voices_playing() == 0, "a voice didn't finish after its length");
    // a full mixer makes way
    for (int i = 0; i < mixer_voices_max + 1; i++) {
        zox_tst_check(zox_mixer_play(samples, zox_tst_mixer_length - i, NULL) >= 0, "sound [%i] didn't play", i);
    }
    zox_tst_check(zox_mixer_voices_playing() == mixer_voices_max, "[%i] voices playing", zox_mixer_voices_playing());
    // the limiter: 65 sounds of 0.5 add to 32, played under 1
    zox_mixer_render(zox_tst_mixer_out, 10);
    float peak = 0;
    for (int j = 0; j < 20; j++) {
        peak = fmaxf(peak, fabsf(zox_tst_mixer_out[j]));
    }
    zox_tst_check(peak <= 1 && peak > 0.9f, "loud mixing peaks at [%.3f]", peak);
    zox_mixer_stop_all();
    dispose_mixer();
    if (listener_was) {
        zox_mixer_set_listener(listener_position, listener_rotation);
    }
}

// tst: jingles, short tunes for moments (level up, quest taken, handed in), sounds/bop/jingles.c
//  - each: notes in order, in note_frequencies, under 4 seconds, some echoes
//  - it rises (its first notes climb, the lift), and lands on a chord (3 or more notes together)
//  - writes bin/tst/jingles.wav, every jingle in turn, to listen to
//  - run: bash bsh/tst.sh jingles

// mixes a jingle into mix at start (samples), as the mixer would play its delayed notes, returns its end
uint zox_tst_render_jingle(JingleType type, float gain, float* mix, uint start, uint length) {
    static float note_samples[MAX_SOUND_DATA];
    JingleNote notes[jingle_max_notes * 2];
    const int count = jingle_performed(type, notes, jingle_max_notes * 2);
    uint end = start;
    for (int i = 0; i < count; i++) {
        const float seconds = fminf(notes[i].seconds, 2.8f);
        const uint note_length = (uint) (seconds * sound_sample_rate);
        const int index = jingles[type].tonic + notes[i].note;
        generate_instrument_samples(notes[i].instrument, note_frequencies[index], seconds, notes[i].volume * gain, 0, note_samples, note_length);
        const uint at = start + (uint) (notes[i].start * sound_sample_rate);
        for (uint j = 0; j < note_length && at + j < length; j++) {
            mix[at + j] += note_samples[j];
        }
        end = at + note_length > end ? at + note_length : end;
    }
    return end;
}

byte zox_tst_jingles_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_jingles_check(ecs* world) {
    (void) world;
    JingleNote notes[jingle_max_notes * 2];
    for (int t = 0; t < jingle_end; t++) {
        const Jingle* jingle = &jingles[t];
        const int count = jingle_performed(t, notes, jingle_max_notes * 2);
        int unsorted = 0, outside = 0, echoes = count - jingle->count;
        for (int i = 0; i < count; i++) {
            unsorted += i > 0 && notes[i].start < notes[i - 1].start;
            const int index = jingle->tonic + notes[i].note;
            outside += index <= 0 || index >= notes_max;
        }
        zox_tst_check(unsorted == 0, "[%s] has [%i] notes out of order", jingle->name, unsorted);
        zox_tst_check(outside == 0, "[%s] has [%i] notes outside note_frequencies", jingle->name, outside);
        zox_tst_check(echoes > 0, "[%s] has no echoes", jingle->name);
        zox_tst_check(jingle_seconds(t) < 4, "[%s] lasts [%.1fs]", jingle->name, jingle_seconds(t));
        // the lift: its first notes climb
        zox_tst_check(jingle->notes[1].note > jingle->notes[0].note, "[%s] doesn't rise", jingle->name);
        // the landing: a chord, 3 or more notes starting together
        int chord = 0;
        for (int i = 0; i < jingle->count; i++) {
            int together = 0;
            for (int j = 0; j < jingle->count; j++) {
                together += fabsf(jingle->notes[j].start - jingle->notes[i].start) < 0.06f;
            }
            chord = together > chord ? together : chord;
        }
        zox_tst_check(chord >= 3, "[%s] doesn't land on a chord (most together [%i])", jingle->name, chord);
    }
    // listen
    const uint length = (uint) (jingle_end * 4.5f * sound_sample_rate);
    float* mix = calloc(length, sizeof(float));
    uint at = 0;
    for (int t = 0; t < jingle_end; t++) {
        at = zox_tst_render_jingle(t, 0.5f, mix, at, length) + sound_sample_rate / 2;
    }
    zox_tst_write_wav("../tst/jingles.wav", mix, at < length ? at : length);
    free(mix);
}

void zox_tsts_sounds() {
    zox_tst_add("jingles", zox_tst_jingles_ready, zox_tst_jingles_check, 30);
    zox_tst_set_setup("jingles", NULL, 1);
    zox_tst_add("sound_mixer", zox_tst_sound_mixer_ready, zox_tst_sound_mixer_check, 30);
    zox_tst_set_setup("sound_mixer", NULL, 1);
    zox_tst_add("sound_formats", zox_tst_sound_formats_ready, zox_tst_sound_formats_check, 30);
    zox_tst_set_setup("sound_formats", NULL, 1);
    zox_tst_add("sound_instruments", zox_tst_sound_instruments_ready, zox_tst_sound_instruments_check, 30);
    zox_tst_set_setup("sound_instruments", NULL, 1);
}
