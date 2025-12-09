void PlayerGameStateSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        disable_inputs_until_release(world, e, zox_device_mode_none, 1);

        byte mouse_lock = state->value == zox_player_state_playing;
        if (zox_valid(local_mouse)) {
            zox_set(local_mouse, MouseLock, { mouse_lock });
        }
    }
} zoxd_system2(PlayerGameStateSystem);