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

byte zox_tst_sound_instruments_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_sound_instruments_check(ecs* world) {
    (void) world;
    const uint length = (uint) (zox_tst_instrument_seconds * sound_sample_rate);
    float* samples = malloc(length * sizeof(float));
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
    }
    free(samples);
}

void zox_tsts_sounds() {
    zox_tst_add("sound_formats", zox_tst_sound_formats_ready, zox_tst_sound_formats_check, 30);
    zox_tst_set_setup("sound_formats", NULL, 1);
    zox_tst_add("sound_instruments", zox_tst_sound_instruments_ready, zox_tst_sound_instruments_check, 30);
    zox_tst_set_setup("sound_instruments", NULL, 1);
}
