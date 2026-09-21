// NOTE: Player cancel the menu!
zox_sys2(CancelMenuSystem) {
    byte dbg_log = 0;
    byte cancel_type = zox_btn_b;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceButtonType);
    zox_sys_in(ZeviceButton);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceButtonType, type);
        zox_sys_i(ZeviceButton, clicked);
        entity device = zox_get_parent(world, e);
        if (!zox_valid(device)) {
            continue;
        }
        entity target = zox_has(device, EntityTarget) ? zox_getv(device, EntityTarget) : 0;
        if (!target) {
            target = zox_has(e, EntityTarget) ? zox_getv(e, EntityTarget) : 0;
        }
        if (!zox_valid(target)) {
            // zox_log("No Raycast Target");
            continue;
        }
        entity menu = zox_get_parent_by_id(world, target, zox_id(Window));
        if (!zox_valid(menu)) {
            continue;
        }
        entity close_button = zox_get_child_by_id_recursive(world, menu, zox_id(CloseButton));
        if (!zox_valid(close_button)) {
            // zox_log("No CloseButton found.");
            continue;
        }
        if (dbg_log) {
            if (devices_get_released_this_frame(clicked->value)) {
                zox_log("Button Released with type [%i]", type->value);
            }
        }
        byte is_cancel = 0;
        if (type->value == cancel_type) {
            if (devices_get_released_this_frame(clicked->value)) {
                is_cancel = 1;
            }
        }
        // released
        if (is_cancel) {
            // zox_log("Canceling Menu [%s]", zox_get_name(menu));
            entity player = zox_get_parent(world, device);
            // clicked state?
            if (dbg_log) {
                zox_log("Player [%s] Clicking [%s]", zox_getn(player), zox_getn(close_button), zox_getn(menu));
            }
            on_element_clicked(world, player, close_button);
            on_element_released(world, player, close_button);
        }
    }
} zox_sys_end(CancelMenuSystem);

// NOTE: Player cancel the menu! (from keyboard)
zox_sys2(KeyboardCancelMenuSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Keyboard);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Keyboard, keyboard);
        entity player = zox_get_parent(world, e);
        entity target = 0;
        /*entity target = zox_has(player, EntityTarget) ? zox_getv(player, EntityTarget) : 0;
        if (!target) {
            target = zox_has(e, EntityTarget) ? zox_getv(e, EntityTarget) : 0;
        }*/
        // NOTE: Should be on the mouse pointer
        if (!target) {
            iter it = zox_children(world, player);
            while (zox_children_next(it)) {
                for (int j = 0; j < it.count; j++) {
                    entity device = it.entities[j];
                    if (!zox_has(device, Device) ||
                        zox_has(device, Disabled))
                    {
                        continue;
                    }
                    if (zox_has(device, EntityTarget)) {
                        target = zox_getv(e, EntityTarget);
                        if (zox_valid(target)) {
                            break;
                        }
                    }
                    iter it2 = zox_children(world, device);
                    while (zox_children_next(it2)) {
                        for (int k = 0; k < it2.count; k++) {
                            entity zevice = it2.entities[k];
                            if (!zox_has(zevice, Zevice)) {
                                continue;
                            }
                            if (zox_has(zevice, Disabled)) {
                                continue;
                            }
                            if (zox_has(zevice, EntityTarget)) {
                                target = zox_getv(zevice, EntityTarget);
                                if (zox_valid(target)) {
                                    break;
                                }
                            }
                        }
                        if (zox_valid(target)) {
                            break;
                        }
                    }
                    if (zox_valid(target)) {
                        break;
                    }
                }
                if (zox_valid(target)) {
                    break;
                }
            }
        }
        if (!zox_valid(target)) {
            if (dbg_log) {
                zox_log("Keyboard found no No EntityTarget");
            }
            continue;
        }
        entity menu = zox_get_parent_by_id(world, target, zox_id(Window));
        if (!zox_valid(menu)) {
            continue;
        }
        entity close_button = zox_get_child_by_id_recursive(world, menu, zox_id(CloseButton));
        if (!zox_valid(close_button)) {
            // zox_log("No CloseButton found.");
            continue;
        }
        byte is_cancel = 0;
        if (keyboard->escape.pressed_this_frame) {
            is_cancel = 1;
        }
        // released
        if (is_cancel) {
            // clicked state?
            if (dbg_log) {
                zox_log("Player [%s] Clicking [%s]", zox_getn(player), zox_getn(close_button), zox_getn(menu));
            }
            on_element_clicked(world, player, close_button);
            on_element_released(world, player, close_button);
        }
    }
} zox_sys_end(KeyboardCancelMenuSystem);

