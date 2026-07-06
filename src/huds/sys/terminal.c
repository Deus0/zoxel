zox_sys2(PlayerTerminalSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CanvasLink);
    zox_sys_in(DeviceMode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CanvasLink, canvas);
        zox_sys_i(DeviceMode, device_mode);
        if (device_mode->value != zox_device_mode_keyboardmouse) {
            continue;
        }
        byte is_toggle_terminal = 0;
        entity keyboards[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, keyboards, zox_children_capacity, zox_id(Keyboard));
        for (uint j = 0; j < length; j++) {
            entity e2 = keyboards[j];
            if (!zox_valid(e2) || zox_getv(e2, DeviceDisabled)) {
                continue;
            }
            if (!zox_has(e2, Keyboard)) {
                continue;
            }
            zox_geter(e2, Keyboard, keyboard);
            if (keyboard->back_quote.pressed_this_frame) {
                is_toggle_terminal = 1;
                break;
            }
        }
        if (!is_toggle_terminal) {
            continue;
        }
        entity menu = zox_get_child_by_id(world, canvas->value, zox_id(MenuTerminal));
        if (zox_valid(menu)) {
            zox_delete(menu);
        } else {
            spawn_menu_terminal(world, canvas->value);
        }
    }
} zox_sys_end(PlayerTerminalSystem);
