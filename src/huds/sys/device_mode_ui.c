// handles changes in device mode state
zox_sys2(DeviceModeUISystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceMode);
    zox_sys_in(DeviceModeDirty);
    zox_sys_in(GameLink);
    zox_sys_in(CanvasLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GameLink, game);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_i(DeviceModeDirty, dmode_new);
        zox_sys_i(DeviceMode, dmode);

        if (!dmode_new->value || dmode_new->value == dmode->value) {
            return;
        }

        byte game_state = 0;
        if (game->value) {
            game_state = zox_get_value(game->value, GameState);
        }
        // handle previous mode
        if (dmode->value == zox_device_mode_touchscreen) {
            SDL_ShowCursor(SDL_DISABLE);
            if (game_state == zox_game_playing) {
                dispose_menu_game_touch(world, e);
            }
        } else if (dmode->value == zox_device_mode_keyboardmouse) {
            SDL_ShowCursor(SDL_ENABLE);
        }
        // handle new mode
        /*if (dmode_new->value == zox_device_mode_gamepad) {
            zox_sys_e();
            raycaster_select_first_button(world, e, canvas->value);
        } else */
        if (dmode_new->value == zox_device_mode_keyboardmouse) {
            raycaster_select_element(world, e, 0);
            SDL_ShowCursor(SDL_DISABLE);
        } else if (dmode_new->value == zox_device_mode_touchscreen) {
            if (game_state == zox_game_playing) {
                spawn_in_game_ui_touch(world, e, canvas->value);
            }
        }
        // todo: this isnt being called, fix it!
        spawn_device_gizmo(world, canvas->value, dmode_new->value);
        menu_start_triggered(world, e, canvas->value);
    }
} zox_sys_end(DeviceModeUISystem);