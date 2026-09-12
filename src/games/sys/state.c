extern entity local_terrain;
extern entity get_terrain_id();

zox_sys2(GameStateSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameStateEvent);
    zox_sys_out(GameState);
    zox_sys_out(GameStateTarget);
    zox_sys_out(LastGameState);
    zox_sys_out(GameStateDirty);
    zox_sys_out(GameStateTime);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GameStateEvent, event);
        zox_sys_o(GameState, state);
        zox_sys_o(GameStateTarget, target);
        zox_sys_o(LastGameState, last);
        zox_sys_o(GameStateDirty, dirty);
        zox_sys_o(GameStateTime, time);
        if (dirty->value || // != zox_dirty_active ||
            target->value == state->value
        ) {
            continue;
        }
        entity realm = zox_get_child_by_id(
            world,
            e,
            zox_id(Realm));
        // NOTE: Handles Game State Transition Checks
        byte is_update = 0;
        if (target->value == zox_game_load_fading) {
            // NOTE: Waits for fade to finish
            double time_passed = zox_current_time - time->value;
            if (zox_valid(realm) && time_passed >= 3) {
                // zox_log("Time Passed [%f] -> [%f] - %f", time->value, zox_current_time, time_passed);
                is_update = 1;
            }
        } else if (state->value == zox_game_loading_realm) {
            // NOTE: Waits for GenerateRealm to finish
            if (zox_valid(realm) && !zox_getv(realm, GenerateRealm)) {
                is_update = 1;
            }
        } else if (state->value == zox_game_loading_terrain) {
            // NOTE: Waits for GenerateRealm to finish
            entity terrain = zox_get_child_by_id(
                world,
                realm,
                get_terrain_id());
            if (!zox_valid(terrain)) {
                zox_loge("Terrain not found in Realm [%s]:[%s]",
                    zox_getn(realm),
                    zox_getn(get_terrain_id()));
                is_update = 1;
            } else if (!zox_has(terrain, Loading)) {
                is_update = 1;
            } else {
                if (dbg_log) {
                    zox_log("Game Waiting on Loading Terrain [%s]",
                        zox_getn(terrain));
                }
            }
        } else {
            is_update = 1;
        }
        if (!is_update) {
            continue;
        }
        byte old_state = state->value;
        byte new_state = target->value;
        last->value = state->value;
        state->value = new_state;
        dirty->value = zox_dirty_trigger;
        time->value = zox_current_time;
        for (int j = 0; j < event->count; j++) {
            if (event->value[j].fun == NULL) {
                zox_loge("Game [%s] StateEvent Null [%i]",
                    zox_sys_e_name,
                    j);
                continue;
            }
            event->value[j].fun(
                world,
                e,
                state->value);
        }
        // Start Loading Realm after faded
        if (state->value == zox_game_load_start) {
            target->value = zox_game_load_fading;
        } else if (state->value == zox_game_load_fading) {
            target->value = zox_game_load_faded;
        } else if (state->value == zox_game_load_faded) {
            target->value = zox_game_loading_realm;
            zox_setv(realm, GenerateRealm, zox_generate_realm_start);
        } else if (state->value == zox_game_loading_realm) {
            target->value = zox_game_terrain_spawn;
        }  else if (state->value == zox_game_terrain_spawn) {
            target->value = zox_game_loading_terrain;
        } else if (state->value == zox_game_loading_terrain) {
            target->value = zox_game_load_end;
        } else if (state->value == zox_game_state_play_begin) {
            target->value = zox_game_state_playing;
        } else if (state->value == zox_game_state_respawn_on_pause) {
            // special edge case
            target->value = zox_game_state_playing;
        }
        // Remove realm (TODO: Move to realm event
        else if (state->value == zox_game_state_the_end) {
            target->value = zox_game_state_the_clean;
            if (dbg_log) {
                zox_log("Game Set To [Clean] [%s]",
                    zox_getn(e));
            }
        } else if (state->value == zox_game_state_the_clean) {
            local_terrain = 0;
            target->value = zox_game_none; // start;
            if (dbg_log) {
                zox_log("Game Set To [Start] [%s]",
                    zox_getn(e));
            }
            // zox_setv(e, RealmLink, 0);
            if (!zox_valid(realm)) {
                zox_loge("Realm Invalid in end game");
            } else {
                zox_log("Destroying The Realm [%s]",
                    zox_getn(realm));
                zox_delete(realm);
            }
        }
        if (dbg_log) {
            zox_log("[%f] Game State [%i] -> [%i] (-> [%i])",
                time->value,
                old_state,
                new_state,
                target->value);
        }
    }
} zox_sys_end(GameStateSystem);
