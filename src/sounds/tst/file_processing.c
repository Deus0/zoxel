// tst: sound_file_processing, two pitch-shifted instances process independently of their source clip.
static byte zox_tst_sound_file_ready(ecs* world) {
    return zox_valid(prefab_sound_filepath) && zox_valid(prefab_sound);
}

static void zox_tst_sound_file_processing_check(ecs* world) {
    const float original[] = { 0, 0.5f, -0.5f, 0.25f, 1, -1 };
    const uint length = sizeof(original) / sizeof(original[0]);
    const float seconds = length / (float) sound_sample_rate;
    const entity clip = spawn_sound_filepath(world, prefab_sound_filepath, original, length, seconds);
    const entity fast = spawn_sound_from_file(world, prefab_sound, clip, 440, 0.5f);
    const entity slow = spawn_sound_from_file(world, prefab_sound, clip, 110, 0.25f);
    zox_tst_check(zox_valid(clip) && zox_valid(fast) && zox_valid(slow), "the sound clip or one of its instances failed to spawn");
    if (!zox_valid(clip) || !zox_valid(fast) || !zox_valid(slow)) {
        return;
    }
    const byte old_nosounds = nosounds;
    const byte old_audio_enabled = audio_enabled;
    nosounds = 0;
    audio_enabled = 0;
    zox_run(world, zox_id(sound_process_system));
    audio_enabled = old_audio_enabled;
    nosounds = old_nosounds;

    const SoundData* source = zox_get(clip, SoundData);
    const SoundData* faster = zox_get(fast, SoundData);
    const SoundData* slower = zox_get(slow, SoundData);
    const byte valid_lengths = source->value && faster->value && slower->value &&
        source->length == length && faster->length == length && slower->length == length;
    zox_tst_check(valid_lengths && zox_getv(fast, SoundLength) == seconds && zox_getv(slow, SoundLength) == seconds,
        "processing changed the clip length or its instance metadata");
    zox_tst_check(source->value != faster->value && source->value != slower->value && faster->value != slower->value,
        "file sound instances shared their sample storage");
    zox_tst_check(!zox_has(fast, ProcessSound) && !zox_has(slow, ProcessSound), "processing left an instance queued to process again");
    if (!valid_lengths) {
        return;
    }
    const float expected_fast[] = { 0, 0.125f, 0.25f, 0, -0.25f, -0.0625f };
    const float expected_slow[] = { 0, -0.125f, 0.25f, 0, 0, 0 };
    for (uint i = 0; i < length; i++) {
        zox_tst_check(source->value[i] == original[i], "processing changed source sample [%u]", i);
        zox_tst_check(faster->value[i] == expected_fast[i], "fast sample [%u] was [%.4f], expected [%.4f]", i, faster->value[i], expected_fast[i]);
        zox_tst_check(slower->value[i] == expected_slow[i], "slow sample [%u] was [%.4f], expected [%.4f]", i, slower->value[i], expected_slow[i]);
    }
}
