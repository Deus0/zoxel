void aura_sound_system(iter* it) {
    zox_sys_on_begin();
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
    zox_sys_on_end();
} zoxd_system(aura_sound_system);