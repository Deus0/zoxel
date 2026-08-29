
void player_state_crosshair(ecs* world, entity player, byte state) {
    byte dbg_log = 0;
    byte is_spawn = state == zox_player_state_play_begin;
    byte is_destroy =
        state == zox_player_state_pause_begin ||
        state == zox_player_state_respawn_begin;
    if (!is_destroy && !is_spawn) {
        return;
    }
    entity canvas = zox_getv(player, CanvasLink);
    // entity character = zox_getv(player, CharacterLink);
    if (!zox_valid(canvas)) {
        return;
    }
    entity ui = zox_get_child_by_id(
        world,
        canvas,
        zox_id(Crosshair));
    if (is_spawn && !zox_valid(ui)) {
            int2 size = int2_single(crosshair_size * ui_scale);
            entity crosshair = spawn_ui(
                world,
                prefab_crosshair,
                canvas,
                float2_half,
                int2_zero,
                size,
                size);
            zox_set_unique_name(crosshair, "crosshair");
        if (dbg_log) {
            zox_log("Spawned [Crosshair] for [%s]",
                zox_getn(player));
        }
    } else if (is_destroy && zox_valid(ui)) {
        if (dbg_log) {
            zox_log("Destroying [Crosshair] [%s] for [%s]",
                zox_getn(ui),
                zox_getn(player));
        }
        zox_delete(ui);
    }
}

// NOTE: Spawn/Destroy [Crosshair] UI for Game UI
/*zox_sys2(PlayerCrosshairSystem) {
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
            // zox_logw("Canvas is missing from Player [PlayerUIGamePauseSystem]");
            continue;
        }
        entity crosshair = zox_get_child_by_id(world, canvas->value, zox_id(Crosshair));
        if (state->value == zox_player_state_pause_begin || state->value == zox_player_state_respawn_begin) {
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
                entity crosshair = spawn_ui(
                    world,
                    prefab_crosshair,
                    canvas->value,
                    float2_half,
                    int2_zero,
                    size,
                    size);
                zox_set_unique_name(crosshair, "crosshair");
            }
        }
    }
} zox_sys_end(PlayerCrosshairSystem);*/
