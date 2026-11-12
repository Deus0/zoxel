zox_sys2(JumpSoundSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(JumpState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(JumpState, jumpState);
        if (jumpState->value == jump_state_active) {
            spawn_sound_effect(world, sound_effect_jump, e);
        }
    }
} zox_sys_end(JumpSoundSystem);
