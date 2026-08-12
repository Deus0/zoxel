// TODO: Make it louder based on velocity of land
zox_sys2(LandSoundSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LandState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LandState, state);
        if (state->value != land_state_active) {
            continue;
        }
        double volume = randf_range(1.6f, 2.0f) * get_volume_sfx();
        double length = randf_range(0.32f, 0.38f);
        byte frequency = rand_range(16, 22);
        byte instrument = instrument_edm; // instrument_edm;
        spawn_sound_generated(
            world,
            prefab_sound_generated,
            instrument,
            note_frequencies[frequency],
            length,
            volume
        );
    }
} zox_sys_end(LandSoundSystem);