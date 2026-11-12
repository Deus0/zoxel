zox_sys2(AuraSoundSystem) {
    // const float popup_spawn_y = 0.18f;
    const double volume = get_volume_sfx();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Activate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Activate, activate);
        if (activate->value == zox_dirty_active) {
            set_sound_source(world, spawn_sound_generated(
                world,
                prefab_sound_generated,
                instrument_violin,
                note_frequencies[28],
                0.6,
                volume
            ), zox_get_parent(world, e));
        }
    }
} zox_sys_end(AuraSoundSystem);