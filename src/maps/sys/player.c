// NOTE: Adds/Removes Minimap to Game UI
zox_sys2(PlayerMinimapSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(CanvasLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(CanvasLink, canvas);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(canvas->value)) {
            // zox_logw("Canvas is missing from Player [PlayerUIGamePauseSystem]");
            continue;
        }
        entity minimap = zox_get_child_by_id(world, canvas->value, zox_id(Minimap));
        if (state->value == zox_player_state_pause_begin || state->value == zox_player_state_respawn_begin) {
            if (zox_valid(minimap)) {
                if (dbg_log) {
                    zox_log("Destroying Player UI [%s]: [Minimap]:[%s]", zox_get_name(e), zox_get_name(minimap));
                }
                zox_delete(minimap);
            }
        } else if (state->value == zox_player_state_play_begin) {
            if (!zox_valid(minimap)) {
                if (dbg_log) {
                    zox_log("Spawning Player UI [%s]: [Minimap]", zox_get_name(e));
                }
                entity game = zox_get_parent(world, e);
                if (!zox_valid(game)) {
                    zox_loge("Player [%s] has invalid Game.", zox_get_name(e));
                    continue;
                }
                entity realm = zox_getv(game, RealmLink);
                if (!zox_valid(realm) || !zox_has(realm, TerrainLink)) {
                    zox_loge("Game [%s] has invalid Realm.", zox_get_name(game));
                    continue;
                }
                entity terrain = zox_getv(realm, TerrainLink);
                spawn_minimap(world, canvas->value, e, terrain);
            }
        }
    }
} zox_sys_end(PlayerMinimapSystem);
