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
            entity device = devices->value[j];

            if (!zox_valid(device) || zox_gett_value(device, DeviceDisabled)) {
                continue;
            }

            if (zox_has(device, Keyboard)) {
                zox_geter(device, Keyboard, keyboard);
                if (keyboard->escape.pressed_this_frame
                    /* || (!keyboard->left_alt.is_pressed && !keyboard->right_alt.is_pressed && keyboard->enter.pressed_this_frame)*/
                ) {
                    did_toggle_pause = 1;
                    break;
                }
            } else if (zox_has(device, Gamepad)) {
                zox_geter(device, Children, zevices);

                for (int k = 0; k < zevices->length; k++) {
                    entity zevice = zevices->value[k];

                    if (!zox_has(zevice, ZeviceButton)) {
                        continue;
                    }

                    zox_geter_value(zevice, ZeviceDisabled, byte, disabled);
                    if (disabled) {
                        continue;
                    }

                    zox_geter_value(zevice, DeviceButtonType, byte, type);
                    if (type == zox_device_button_start || type == zox_device_button_select) {
                        zox_geter_value(zevice, ZeviceButton, byte, value);
                        if (devices_get_pressed_this_frame(value)) {
                            did_toggle_pause = 1;
                        }
                    }
                }
            }
        }
        if (did_toggle_pause) {
            byte is_paused = game_state == zox_game_paused;
            zox_set(game->value, GameStateTarget, { is_paused ? zox_game_playing : zox_game_paused });
        }
    }

} zox_sys_end(PlayerPauseSystem);