// handles changes in device mode state from a Player

void set_mouse_visible(byte state) {
    #ifdef zox_sdl
    if (state) {
        SDL_ShowCursor(SDL_ENABLE);
    } else {
        SDL_ShowCursor(SDL_DISABLE);
    }
    #endif
}
zox_sys2(DeviceModeUISystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceModeDirty);
    zox_sys_in(LastDeviceMode);
    zox_sys_in(DeviceMode);
    zox_sys_in(GameLink);
    zox_sys_in(CanvasLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceModeDirty, dirty);
        zox_sys_i(LastDeviceMode, old);
        zox_sys_i(DeviceMode, new);
        zox_sys_i(GameLink, game);
        zox_sys_i(CanvasLink, canvas);
        if (!dirty->value) {
            return;
        }
        byte mouse_visible = 1;
        byte game_state = 0;
        if (game->value) {
            game_state = zox_get_value(game->value, GameState);
        }
        // handle previous mode
        if (old->value == zox_device_mode_touchscreen) {
            if (game_state == zox_game_state_playing) {
                dispose_menu_game_touch(world, e);
            }
        }
        // NOTE: Handle New Mode
        if (new->value == zox_device_mode_keyboardmouse) {
            raycaster_select_element(world, e, 0);
            mouse_visible = 0;
        } else if (new->value == zox_device_mode_touchscreen) {
            if (game_state == zox_game_state_playing) {
                spawn_in_game_ui_touch(world, e, canvas->value);
            } else {
                zox_log("Game isnt playing.");
            }
            mouse_visible = 0;
        }
        set_mouse_visible(mouse_visible);
        spawn_device_gizmo(world, canvas->value, new->value);
        menu_start_triggered(world, e, canvas->value);
        if (dbg_log) {
            zox_log("New Device Mode Detected [%i] -> [%i], mouse_visible [%i]", old->value, new->value, mouse_visible);
        }
    }
} zox_sys_end(DeviceModeUISystem);
