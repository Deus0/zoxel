// tst: sound_wav_decode, SDL-loaded PCM frames are normalized and downmixed with the same duration.
static byte zox_tst_sound_write_fixture(const char* path, const byte* data, size_t length) {
    FILE* file = fopen(path, "wb");
    if (!file) {
        return 0;
    }
    const byte written = fwrite(data, 1, length, file) == length;
    return fclose(file) == 0 && written;
}

static void zox_tst_sound_wav_samples(const char* path, const float* expected, uint count) {
    float* samples = NULL;
    uint frames = 0;
    float seconds = 0;
    const byte loaded = load_wav_data(path, &samples, &frames, &seconds, 0);
    zox_tst_check(loaded && samples && frames == count, "[%s] decoded [%u] mono samples, expected [%u]", path, frames, count);
    if (loaded && samples && frames == count) {
        zox_tst_check(fabsf(seconds - count / (float) sound_sample_rate) < 1e-7f, "[%s] decoded the wrong duration", path);
        for (uint i = 0; i < count; i++) {
            zox_tst_check(fabsf(samples[i] - expected[i]) < 1e-6f, "[%s] sample [%u] was [%.6f], expected [%.6f]",
                path, i, samples[i], expected[i]);
        }
    }
    free(samples);
}

static void zox_tst_sound_wav_decode_check(ecs* world) {
    (void) world;
    // PCM16 stereo at 44100 Hz: opposing channels, negative, asymmetric, then both near full scale.
    static const byte stereo[] = {
        'R','I','F','F', 52,0,0,0, 'W','A','V','E', 'f','m','t',' ', 16,0,0,0,
        1,0, 2,0, 0x44,0xac,0,0, 0x10,0xb1,2,0, 4,0, 16,0,
        'd','a','t','a', 16,0,0,0, 0,0x40,0,0xc0, 0,0x80,0,0, 0,0x40,0,0x20, 0xff,0x7f,0xff,0x7f
    };
    static const float stereo_expected[] = { 0, -0.5f, 0.375f, 32767.0f / 32768.0f };
    const char* stereo_path = zox_tst_path("stereo_pcm16.wav");
    const byte wrote_stereo = zox_tst_sound_write_fixture(stereo_path, stereo, sizeof(stereo));
    zox_tst_check(wrote_stereo, "could not write the stereo PCM fixture");
    if (wrote_stereo) {
        zox_tst_sound_wav_samples(stereo_path, stereo_expected, 4);
    }

    // PCM8 mono at 44100 Hz: unsigned endpoints and midpoint.
    static const byte mono[] = {
        'R','I','F','F', 40,0,0,0, 'W','A','V','E', 'f','m','t',' ', 16,0,0,0,
        1,0, 1,0, 0x44,0xac,0,0, 0x44,0xac,0,0, 1,0, 8,0,
        'd','a','t','a', 4,0,0,0, 0,128,192,255
    };
    static const float mono_expected[] = { -1, 0, 0.5f, 127.0f / 128.0f };
    const char* mono_path = zox_tst_path("mono_pcm8.wav");
    const byte wrote_mono = zox_tst_sound_write_fixture(mono_path, mono, sizeof(mono));
    zox_tst_check(wrote_mono, "could not write the mono PCM fixture");
    if (wrote_mono) {
        zox_tst_sound_wav_samples(mono_path, mono_expected, 4);
    }

    const float float_samples[] = { -0.75f, -0.125f, 0.25f, 0.875f };
    const char* float_path = zox_tst_path("mono_float32.wav");
    zox_tst_write_wav(float_path, float_samples, 4);
    zox_tst_sound_wav_samples(float_path, float_samples, 4);
}
