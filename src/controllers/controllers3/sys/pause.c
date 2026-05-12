// Player presses input which pauses game
zox_sys2(PlayerPauseSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(DeviceLinks);
    zox_sys_in(GameLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(PlayerState, state);
        zox_sys_i(DeviceLinks, devices);
        zox_sys_i(GameLink, game);
        if (state->value != zox_player_state_playing && state->value != zox_player_state_paused) {
            continue;
        }
        if (!zox_valid(game->value)) {
            continue;
        }
        zox_geter_value(game->value, GameState, byte, game_state);
        if (!(game_state == zox_game_playing || game_state == zox_game_paused)) {
            continue;
        }
        byte did_toggle_pause = 0;
        for (int j = 0; j < devices->length; j++) {
            entity e2 = devices->value[j];
            if (!zox_valid(e2) || zox_gett_value(e2, DeviceDisabled)) {
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
                    if (type == zox_device_button_start) {
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
            byte is_paused = game_state == zox_game_paused;
            zox_set(game->value, GameStateTarget, { is_paused ? zox_game_playing : zox_game_paused });
        }
    }

} zox_sys_end(PlayerPauseSystem);
