// Spawn the UIs
zox_sys2(PlayerTouchUISystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(CanvasLink);
    zox_sys_in(DeviceMode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_i(DeviceMode, device_mode);
        if (state->value == zox_player_state_play_begin && dirty->value == zox_dirty_active) {
            if (device_mode->value == zox_device_mode_touchscreen || zox_dbg_touch_with_mouse) {
                spawn_in_game_ui_touch(world, e, canvas->value);
            }
        }
    }
} zox_sys_end(PlayerTouchUISystem);
