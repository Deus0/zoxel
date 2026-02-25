zox_sys2(GameStateSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RealmLink);
    zox_sys_out(GameState);
    zox_sys_out(GameStateTarget);
    zox_sys_out(GameStateDirty);
    zox_sys_out(GameStateTime);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RealmLink, realm);
        zox_sys_o(GameState, state);
        zox_sys_o(GameStateTarget, target);
        zox_sys_o(GameStateDirty, dirty);
        zox_sys_o(GameStateTime, time);

        if (dirty->value || target->value == state->value) {
            continue;
        }

        // handle different next conditions
        byte is_update = 0;
        // NOTE: Waits for fade to finish
        if (target->value == zox_game_load_fading) {
            double time_passed = zox_current_time - time->value;
            if (zox_valid(realm->value) && time_passed >= 3) {
                // zox_log("Time Passed [%f] -> [%f] - %f", time->value, zox_current_time, time_passed);
                is_update = 1;
            }
        }
        // NOTE: Waits for GenerateRealm to finish
        else if (target->value == zox_game_loading_realm) {
            if (zox_valid(realm->value) && !zox_gett_value(realm->value, GenerateRealm)) {
                is_update = 1;
            }
        }
        else {
            is_update = 1;
        }

        if (!is_update) {
            continue;
        }

        byte old_state = state->value;
        byte new_state = target->value;
        state->value = new_state;
        trigger_event_game(world, e, old_state, target->value);
        // set game state dirty
        dirty->value = zox_dirty_trigger;
        time->value = zox_current_time;
        zox_logv("[%f] Game State [%i] -> [%i]", time->value, old_state, new_state);

        // Start Loading Realm after faded
        if (state->value == zox_game_load_start) {
            target->value = zox_game_load_fading;
        }
        else if (state->value == zox_game_load_fading) {
            target->value = zox_game_load_faded;
        }
        else if (state->value == zox_game_load_faded) {
            target->value = zox_game_loading_realm;
            zox_set(realm->value, GenerateRealm, { zox_generate_realm_start });
        }
        // finished loading realm
        else if (state->value == zox_game_loading_realm) {
            target->value = zox_game_playing_start;
        }
        else if (state->value == zox_game_playing_start) {
            target->value = zox_game_playing;
        }

    }
} zox_sys_end(GameStateSystem);

// when state changes, we can do events here
//      game has children which includes all players
//      game_state_start > game_state_menu = load main menu for player 1
//      game_state_menu > pre play = unload main menu, load a countdown / load ui
//      game_state_pre_play > play = load game, load player characters
//      game_state_play > game_state_paused = pause entities in game
//      game_state_paused > game_state_play = unpause entities
//      game_state_play | game_state_paused > game_state_end = go to exit ui
//      game_state_end > game_state_menu = return to main menu scene + ui
