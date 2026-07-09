zox_sys2(GameStateSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RealmLink);
    zox_sys_out(GameState);
    zox_sys_out(GameStateTarget);
    zox_sys_out(LastGameState);
    zox_sys_out(GameStateDirty);
    zox_sys_out(GameStateTime);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(RealmLink, realm);
        zox_sys_o(GameState, state);
        zox_sys_o(GameStateTarget, target);
        zox_sys_o(LastGameState, last);
        zox_sys_o(GameStateDirty, dirty);
        zox_sys_o(GameStateTime, time);
        if (dirty->value || target->value == state->value) {
            continue;
        }
        // handle different next conditions
        byte is_update = 0;
        if (target->value == zox_game_load_fading) {
            // NOTE: Waits for fade to finish
            double time_passed = zox_current_time - time->value;
            if (zox_valid(realm->value) && time_passed >= 3) {
                // zox_log("Time Passed [%f] -> [%f] - %f", time->value, zox_current_time, time_passed);
                is_update = 1;
            }
        } else if (target->value == zox_game_loading_realm) {
            // NOTE: Waits for GenerateRealm to finish
            if (zox_valid(realm->value) && !zox_getv(realm->value, GenerateRealm)) {
                is_update = 1;
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
        // trigger_event_game(world, e, old_state, target->value);
        if (dbg_log) {
            zox_log("[%f] Game State [%i] -> [%i]", time->value, old_state, new_state);
        }
        // Start Loading Realm after faded
        if (state->value == zox_game_load_start) {
            target->value = zox_game_load_fading;
        } else if (state->value == zox_game_load_fading) {
            target->value = zox_game_load_faded;
        } else if (state->value == zox_game_load_faded) {
            target->value = zox_game_loading_realm;
            zox_set(realm->value, GenerateRealm, { zox_generate_realm_start });
        } else if (state->value == zox_game_loading_realm) {
            // finished loading realm
            target->value = zox_game_state_play_begin;
        } else if (state->value == zox_game_state_play_begin) {
            target->value = zox_game_state_playing;
        } else if (state->value == zox_game_state_respawn_on_pause) {
            // special edge case
            target->value = zox_game_state_playing;
        }
    }
} zox_sys_end(GameStateSystem);
