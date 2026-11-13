void zevice_click_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(EntityTarget);
    zox_sys_out(ClickingEntity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity device = zox_get_link(world, e, DeviceLink);
        zox_sys_i(EntityTarget, target);
        zox_sys_o(ClickingEntity, clicked);
        if (!zox_valid(device)) {
            zox_loge(" device null from zevice [%lu]", e);
            continue;
        }
        entity player = zox_get_parent(world, device);
        if (!zox_valid(player)) {
            continue;
        }
        if (zox_has(device, Disabled)) {
            continue;
        }
        byte device_mode = zox_getv(player, DeviceMode);
        byte click_type = 0;
        if (zox_has(e, ZevicePointer)) {
            byte click_value = zox_getv(e, ZevicePointer);
            if (devices_get_pressed_this_frame(click_value)) {
                click_type = 1;
            } else if (devices_get_released_this_frame(click_value)) {
                click_type = 2;
            }
        }
        if (zox_has(e, ZeviceButton)) {
            const DeviceButtonType* deviceButtonType = zox_get(e, DeviceButtonType);
            if (deviceButtonType->value == zox_btn_a) {
                byte click_value = zox_getv(e, ZeviceButton);
                if (devices_get_pressed_this_frame(click_value)) {
                    click_type = 1;
                } else if (devices_get_released_this_frame(click_value)) {
                    click_type = 2;
                }
            }
        }
        // the mouse's right button right clicks on press
        if (zox_has(e, ZevicePointerRight) &&
            devices_get_pressed_this_frame(zox_getv(e, ZevicePointerRight)))
        {
            on_element_right_pressed(world, player, target->value);
        }
        // used for virtual joysticks to see if a t arget was raycasted, todo: move to raycast system
        // result->value = target->value;
        // released
        if (click_type == 0) {
            continue;
        }
        if (click_type == 1) {
            clicked->value = zox_valid(target->value) ? target->value : 0;
            on_element_pressed(world, player, clicked->value);
            // Drag
            if (zox_valid(target->value) && zox_has(target->value, Dragable)) {
                byte drag_mode = zox_drag_mode_none;
                if (device_mode == zox_device_mode_keyboardmouse) {
                    drag_mode = zox_drag_mode_mouse;
                } else if (device_mode == zox_device_mode_touchscreen) {
                    drag_mode = zox_drag_mode_finger;
                }
                set_element_dragged(world, player, target->value, drag_mode);
                if (dbg_log) {
                    zox_log("Zevice Dragging UI [%s] Mode [%i]", zox_getn(target->value), drag_mode);
                }
            }
        }
        // released
        else if (click_type == 2) {
            if (target->value == clicked->value) {
                on_element_press_released(world, player, target->value);
                if (dbg_log) {
                    zox_log("Zevice Released UI [%s]", zox_getn(target->value));
                }
            }
            clicked->value = 0;
        }
    }
    zox_sys_on_end();
} zoxd_system(zevice_click_system);
