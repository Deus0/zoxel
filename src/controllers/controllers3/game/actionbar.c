
void player_state_actionbar(ecs* world, entity player, byte state) {
    byte dbg_log = 0;
    byte is_spawn = state == zox_player_state_play_begin;
    byte is_destroy = state == zox_player_state_respawn_begin;
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
        zox_id(MenuActions));
    if (is_spawn && !zox_valid(ui)) {
        spawn_menu_actions(world, player);
        if (dbg_log) {
            zox_log("Spawned [MenuActions] for [%s]",
                zox_getn(player));
        }
    } else if (is_destroy && zox_valid(ui)) {
        if (dbg_log) {
            zox_log("Destroying [MenuActions] [%s] for [%s]",
                zox_getn(ui),
                zox_getn(player));
        }
        zox_delete(ui);
    }
}

// NOTE: Spawn/Destroy [Actionbar] UI for Game UI
/*zox_sys2(PlayerActionbarSystem) {
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
            zox_logw("[PlayerActionbarSystem] Invalid Canvas");
            continue;
        }
        entity actionbar = zox_get_child_by_id(world, canvas->value, zox_id(MenuActions));
        if (state->value == zox_player_state_respawn_begin) {
            if (zox_valid(actionbar)) {
                if (dbg_log) {
                    zox_log("- Destroying [actionbar] [%s] for Player [%s]", zox_get_name(actionbar), zox_get_name(e));
                }
                zox_delete(actionbar);
            }
        } else if (state->value == zox_player_state_play_begin) {
            if (!zox_valid(actionbar)) {
                if (dbg_log) {
                    zox_log("+ Spawning [Actionbar] for Player [%s]", zox_get_name(e));
                }
                spawn_menu_actions(world, e);
            }
        }
    }
} zox_sys_end(PlayerActionbarSystem);*/

