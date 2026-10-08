zox_sys2(ClickSoundSystem) {
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        double volume = (0.53 + 0.32 * (rand() % 101) / 100.0) * get_volume_sfx();
        double length = 0.33 + 0.22 * (rand() % 101) / 100.0;
        if (rand() % 100 >= 94) {
            spawn_sound_generated(world, prefab_sound_generated, instrument_violin, note_frequencies[38], length, volume);
        } else {
            int frequency = (int)(22 + 8 * (rand() % 101) / 100.0);
            spawn_sound_generated(world, prefab_sound_generated, instrument_flute, note_frequencies[frequency], length, volume);
        }
    }
} zox_sys_end(ClickSoundSystem);
