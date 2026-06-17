// NOTE: Spawn/Destroy [Crosshair] UI for Game UI
zox_sys2(PlayerCrosshairSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(CanvasLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(CanvasLink, canvas);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(canvas->value)) {
            zox_logw("Canvas is missing from Player [PlayerUIGamePauseSystem]");
            continue;
        }
        entity crosshair = zox_get_child_by_id(world, canvas->value, zox_id(Crosshair));
        if (state->value == zox_player_state_pause_begin || state->value == state->value == zox_player_state_respawn_begin) {
            if (zox_valid(crosshair)) {
                if (dbg_log) {
                    zox_log("- Destroying [Crosshair] [%s] for Player [%s]", zox_get_name(crosshair), zox_get_name(e));
                }
                zox_delete(crosshair);
            }
        } else if (state->value == zox_player_state_play_begin) {
            if (!zox_valid(crosshair)) {
                if (dbg_log) {
                    zox_log("+ Spawning [Crosshair] for Player [%s]", zox_get_name(e));
                }
                int2 size = int2_single(crosshair_size * ui_scale);
                spawn_ui(world, prefab_crosshair, canvas->value, float2_half, int2_zero, size, size);
            }
        }
    }
} zox_sys_end(PlayerCrosshairSystem);
