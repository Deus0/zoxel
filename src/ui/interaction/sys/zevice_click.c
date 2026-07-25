zox_sys2(ZeviceClickSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZeviceDisabled);
    zox_sys_in(DeviceLink);
    zox_sys_in(RaycasterTarget);
    zox_sys_out(ClickingEntity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZeviceDisabled, disabled);
        zox_sys_i(DeviceLink, device);
        zox_sys_i(RaycasterTarget, target);
        zox_sys_o(ClickingEntity, clicked);
        if (disabled->value) {
            continue;
        }
        entity player = zox_get_parent(world, device->value);
        if (!zox_valid(player)) {
            // zox_log(" ! device has null player [%lu]\n", device)
            continue;
        }
        if (!zox_valid(device->value)) {
            zox_loge(" device null from zevice [%lu]", e);
            continue;
        }
        if (zox_getv(device->value, DeviceDisabled)) {
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
            zox_geter(e, DeviceButtonType, deviceButtonType);
            if (deviceButtonType->value == zox_btn_a) {
                byte click_value = zox_getv(e, ZeviceButton);
                if (devices_get_pressed_this_frame(click_value)) {
                    click_type = 1;
                } else if (devices_get_released_this_frame(click_value)) {
                    click_type = 2;
                }
            }
        }
        // used for virtual joysticks to see if a t arget was raycasted, todo: move to raycast system
        // result->value = target->value;
        // released
        if (click_type == 0) {
            continue;
        } else if (click_type == 1) {
            clicked->value = zox_valid(target->value) ? target->value : 0;
            on_element_clicked(world, player, clicked->value);
        }
        if (click_type == 1) { // clicked
            if (zox_valid(target->value) && zox_has(target->value, Dragable)) {
                byte drag_mode = zox_drag_mode_none;
                if (device_mode == zox_device_mode_keyboardmouse) {
                    drag_mode = zox_drag_mode_mouse;
                } else if (device_mode == zox_device_mode_touchscreen) {
                    drag_mode = zox_drag_mode_finger;
                }
                set_element_dragged(world, player, target->value, drag_mode);
            }
        } else if (click_type == 2) { // released
            if (target->value == clicked->value) {
                on_element_released(world, player, target->value);
            }
            clicked->value = 0;
        }
    }
} zox_sys_end(ZeviceClickSystem);
