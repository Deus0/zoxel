// handles changes in device mode state from a Player

// grid like ui
float device_gizmo_fade_time = 3;

// when switch, spawn, destroy after x seconds, add a cooldown on switching devices
entity spawn_device_gizmo(ecs* world, entity canvas, byte device_type) {
    int2 size = int2_single(32 * ui_scale);
    int2 position = int2_single(16 * ui_scale);
    position.x = 32 * ui_scale;   // testing anchor
    float2 position_anchor = float2_zero;
    byte layer = max_layers2D - 2;
    // spawn it
    entity e = spawn_ui(world, prefab_element, canvas, position_anchor, position, size, size);
    zox_name("device_gizmo");
    zox_set(e, Layer2D, { layer });
    zox_set(e, Alpha, { 1 });
    zox_set(e, AnimationStart, { zox_current_time });
    zox_set(e, FadeOutTime, { device_gizmo_fade_time });
    zox_set(e, DestroyInTime, { device_gizmo_fade_time });
    char* texture_name;
    if (device_type == zox_device_mode_gamepad) {
        texture_name = "device_gamepad";
    } else if (device_type == zox_device_mode_keyboardmouse) {
        texture_name = "device_keyboard";
    } else if (device_type == zox_device_mode_touchscreen) {
        texture_name = "device_touchscreen";
    } else {
        texture_name = "device_none";
    }
    clone_texture_file_to_entity(world, e, texture_name);
    // zox_log("+ Device UI [%i] on Canvas [%s]; prefab [%s]", device_type, zox_get_name(canvas), zox_get_name(prefab));
    return e;
}

void set_mouse_visible(byte state) {
#ifdef zox_sdl
    zox_sdl_show_cursor(state);
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
