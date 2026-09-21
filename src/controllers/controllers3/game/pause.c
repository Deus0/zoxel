// Player presses input which pauses game
// NOTE: Must be paused or playing state to toggle pause!
zox_sys2(PlayerPauseSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(PlayerStateDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(PlayerStateDirty, dirty);
        if (dirty->value) {
            continue;
        }
        if (!(state->value == zox_player_state_playing || state->value == zox_player_state_paused)) {
            continue;
        }
        entity game = zox_get_parent(world, e);
        if (!zox_valid(game)) {
            continue;
        }
        zox_geter_value(game, GameState, byte, game_state);
        if (!(game_state == zox_game_state_playing || game_state == zox_game_state_paused)) {
            continue;
        }
        byte did_toggle_pause = 0;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (zox_has(e2, Disabled)) {
                continue;
            }
            uint children_capacity = zox_children_capacity;
            entity children[children_capacity];
            uint children_length = zox_get_children(world, e2, children, children_capacity);
            for (uint k = 0; k < children_length; k++) {
                entity e3 = children[k];
                if (!zox_valid(e3)) {
                    continue;
                }
                zox_geter_value(e3, ZeviceDisabled, byte, disabled);
                if (disabled) {
                    continue;
                }
                if (zox_has(e3, ZeviceButton)) {
                    zox_geter_value(e3, DeviceButtonType, byte, type);
                    if (type == zox_btn_start) {
                        zox_geter_value(e3, ZeviceButton, byte, value);
                        if (devices_get_pressed_this_frame(value)) {
                            did_toggle_pause = 1;
                        }
                    }
                }
            }
            if (zox_has(e2, Keyboard)) {
                zox_geter(e2, Keyboard, keyboard);
                if (keyboard->escape.pressed_this_frame ||
                    keyboard->enter.pressed_this_frame) {
                    did_toggle_pause = 1;
                    break;
                }
            }
        }
        if (did_toggle_pause) {
            byte is_paused = state->value == zox_player_state_paused; // game_state == zox_game_state_paused;
            // byte is_paused = game_state == zox_game_state_paused;
            zox_setv(game, GameStateTarget,
                is_paused ?
                    zox_game_state_playing :
                    zox_game_state_paused);
            if (dbg_log) {
                zox_log("Toggling Pause with state [%i] Game Pausing? [%i]", state->value, is_paused);
            }
        }
    }

} zox_sys_end(PlayerPauseSystem);
