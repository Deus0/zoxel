// tst: sound_voice_lifecycle, voices own their samples across mix blocks and eviction respects remaining time.
static void zox_tst_sound_voice_lifecycle_check(ecs* world) {
    (void) world;
    const float old_volume = mixer_volume;
    const byte old_underwater = sound_underwater;
    sound_underwater = 0;
    mixer_volume = 0.5f;
    dispose_mixer();
    float first[] = { 0.125f, 0.25f, 0.375f, 0.5f };
    float second[] = { -0.25f, 0.125f };
    zox_tst_check(zox_mixer_play(first, 4, NULL) >= 0, "the first voice failed to queue");
    zox_tst_check(zox_mixer_play_delayed(second, 2, NULL, 2.25f / sound_sample_rate) >= 0, "the delayed voice failed to queue");
    memset(first, 0, sizeof(first));
    memset(second, 0, sizeof(second));

    const float expected[] = { 0.0625f, 0.125f, 0.0625f, 0.3125f, 0, 0 };
    float output[6];
    for (int block = 0; block < 2; block++) {
        zox_mixer_render(output, 3);
        for (int frame = 0; frame < 3; frame++) {
            const float value = expected[3 * block + frame];
            zox_tst_check(output[2 * frame] == value && output[2 * frame + 1] == value,
                "mixed frame [%i] was [%.4f, %.4f], expected [%.4f] on both channels",
                3 * block + frame, output[2 * frame], output[2 * frame + 1], value);
        }
        zox_tst_check(zox_mixer_voices_playing() == (block == 0 ? 2 : 0), "unexpected active voice count after block [%i]", block);
    }

    mixer_volume = 1;
    const float quiet[] = { 0.005f, 0.005f, 0.005f, 0.005f };
    const float replacement = -0.125f;
    int shortest = -1;
    for (int i = 0; i < mixer_voices_max; i++) {
        const uint length = i == 5 || i == 7 ? 1 : 4;
        const float delay = i == 5 ? 4.25f / sound_sample_rate : 0;
        const int voice = zox_mixer_play_delayed(quiet, length, NULL, delay);
        if (i == 7) {
            shortest = voice;
        }
    }
    zox_tst_check(zox_mixer_voices_playing() == mixer_voices_max, "the fixture did not fill the mixer");
    const int replaced = zox_mixer_play(&replacement, 1, NULL);
    zox_tst_check(shortest >= 0 && replaced == shortest, "the mixer evicted voice [%i] instead of shortest voice [%i]", replaced, shortest);
    zox_mixer_render(output, 1);
    const float expected_after_eviction = (mixer_voices_max - 2) * quiet[0] + replacement;
    zox_tst_check(fabsf(output[0] - expected_after_eviction) < 1e-5f && fabsf(output[1] - expected_after_eviction) < 1e-5f,
        "the replacement frame retained the evicted voice or played a delayed voice early");
    dispose_mixer();
    mixer_volume = old_volume;
    sound_underwater = old_underwater;
}
