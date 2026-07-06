zox_sys2(DialogueSoundSystem) {
    byte instrument = instrument_edm; // instrument_flute;
    float2 time_range = (float2) { 0.1f, 0.2f };
    float2 volume_range = (float2) { 0.6f, 1.2f };
    byte2 frequency_range = (byte2) { 16, 34 }; // 22
    float2 final_time_range = (float2) { 1.1f, 2.2f };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZigelSpawnedDirty);
    zox_sys_in(AnimateTextEnded);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ZigelSpawnedDirty, state);
        zox_sys_i(AnimateTextEnded, state2);
        if (state->value != zox_dirty_active && state2->value != zox_dirty_active) {
            continue;
        }
        double volume = frand_range(volume_range.x, volume_range.y);
        volume *= get_volume_sfx();
        // (0.53 + 0.32 * (rand() % 101) / 100.0) * get_volume_sfx();
        double length = frand_range(time_range.x, time_range.y);
        // 0.33 + 0.22 * (rand() % 101) / 100.0;
        if (state2->value == zox_dirty_active) {
            length = frand_range(final_time_range.x, final_time_range.y);
        }
        if (rand() % 100 >= 94) {
            spawn_sound_generated(world, prefab_sound_generated, instrument_violin, note_frequencies[38], length, volume);
        } else {
            int frequency = rand_range(frequency_range.x, frequency_range.y);
            spawn_sound_generated(world, prefab_sound_generated, instrument, note_frequencies[frequency], length, volume);
        }
    }
} zox_sys_end(DialogueSoundSystem);
