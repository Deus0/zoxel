zox_sys2(AuraSoundSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Activate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Activate, activate);
        if (activate->value == zox_dirty_active) {
            spawn_sound_effect(world, sound_effect_aura, zox_get_parent(world, e));
        }
    }
} zox_sys_end(AuraSoundSystem);