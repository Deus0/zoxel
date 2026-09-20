void player_state_touch_ui(
    ecs* world,
    entity player,
    byte state)
{
    byte dbg_log = 0;
    byte device_mode = zox_getv(player, DeviceMode);
    byte is_spawn =
        state == zox_player_state_play_begin &&
        (device_mode == zox_device_mode_touchscreen ||
            zox_dbg_touch_with_mouse);
    byte is_destroy = state == zox_player_state_respawn_begin;
    if (!is_destroy && !is_spawn) {
        return;
    }
    entity canvas = zox_get_link(world, player, CanvasLink);
    if (!zox_valid(canvas)) {
        zox_loge("[player_state_touch_ui] Invalid [canvas]");
        return;
    }
    entity ui = zox_get_child_by_id(
        world,
        canvas,
        zox_id(MenuPlayTouch));
    if (is_spawn && !zox_valid(ui)) {
        spawn_in_game_ui_touch(
            world,
            player,
            canvas);
        if (dbg_log) {
            zox_log("Spawned [MenuPlayTouch] for [%s]",
                zox_getn(player));
        }
    } else if (is_destroy && zox_valid(ui)) {
        if (dbg_log) {
            zox_log("Destroying [MenuPlayTouch] [%s] for [%s]",
                zox_getn(ui),
                zox_getn(player));
        }
        zox_delete(ui);
    }
}

// Spawn the UIs
/*zox_sys2(PlayerTouchUISystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(DeviceMode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(DeviceMode, device_mode);
        if (state->value == zox_player_state_play_begin && dirty->value == zox_dirty_active) {
            if (device_mode->value == zox_device_mode_touchscreen || zox_dbg_touch_with_mouse) {
                spawn_in_game_ui_touch(world, e, canvas->value);
            }
        }
    }
} zox_sys_end(PlayerTouchUISystem);*/
