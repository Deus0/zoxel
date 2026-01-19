zox_sys2(PlayerTerminalSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CanvasLink);
    zox_sys_in(DeviceLinks);
    zox_sys_in(DeviceMode);
    zox_sys_out(ElementLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CanvasLink, canvas);
        zox_sys_i(DeviceLinks, devices);
        zox_sys_i(DeviceMode, dmode);
        zox_sys_o(ElementLinks, elements);

        if (dmode->value != zox_device_mode_keyboardmouse) {
            continue;
        }

        byte is_toggle_terminal = 0;
        for (int j = 0; j < devices->length; j++) {
            const entity device = devices->value[j];
            if (!zox_valid(device) || zox_gett_value(device, DeviceDisabled)) {
                continue;
            }
            if (zox_has(device, Keyboard)) {
                zox_geter(device, Keyboard, keyboard)
                if (keyboard->back_quote.pressed_this_frame) {
                    is_toggle_terminal = 1;
                    break;
                }
            }
        }

        if (!is_toggle_terminal) {
            continue;
        }

        entity menu = player_toggle_ui_id(
            world,
            canvas->value,
            elements,
            zox_id(MenuTerminal),
            spawn_menu_terminal);
        if (menu) {
            zox_log("Spawned Player Terminal");
            add_to_ElementLinks(elements, menu);
            zox_set(menu, ElementHolder, { e });
        }

        /*find_child_with_tag(canvas->value, MenuTerminal, menu);
        if (zox_valid(menu)) {
            zox_delete(menu);
        } else {

        }*/
    }
} zox_sys_end(PlayerTerminalSystem);