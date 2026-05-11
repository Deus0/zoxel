zox_sys2(ZeviceClickSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceLink);
    zox_sys_in(RaycasterTarget);
    //  zox_sys_in(WindowRaycasted);
    zox_sys_out(RaycasterResult);
    zox_sys_out(ClickingEntity);
    // zox_sys_out(WindowTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceLink, device);
        zox_sys_i(RaycasterTarget, raycasterTarget);
        // zox_sys_i(WindowRaycasted, windowRaycasted);
        zox_sys_o(RaycasterResult, raycasterResult);
        zox_sys_o(ClickingEntity, clickingEntity);
        // zox_sys_o(WindowTarget, windowTarget);
        if (!zox_valid(device->value)) {
            zox_log_error(" device null from zevice [%lu]", it->entities[i])
            continue;
        }
        if (zox_gett_value(device->value, DeviceDisabled)) {
            continue;
        }
        entity player = zox_get_value(device->value, PlayerLink)
        if (!player) {
            // zox_log(" ! device has null player [%lu]\n", device)
            continue;
        }
        byte device_mode = zox_get_value(player, DeviceMode);
        // entity canvas = zox_get_value(player, CanvasLink)
        byte click_type = 0;
        if (zox_has(e, ZevicePointer)) {
            byte click_value = zox_get_value(e, ZevicePointer);

            if (devices_get_pressed_this_frame(click_value)) click_type = 1;
            else if (devices_get_released_this_frame(click_value)) click_type = 2;
        }
        if (zox_has(e, ZeviceButton)) {
            zox_geter(e, DeviceButtonType, deviceButtonType);
            if (deviceButtonType->value == zox_device_button_a) {
                zox_geter(e, ZeviceDisabled, zeviceDisabled);
                if (!zeviceDisabled->value) {
                    byte click_value = zox_get_value(e, ZeviceButton);

                    if (devices_get_pressed_this_frame(click_value)) {
                        click_type = 1;
                    } else if (devices_get_released_this_frame(click_value)) {
                        click_type = 2;
                    }
                }
            }
        }
        // used for virtual joysticks to see if a t arget was raycasted, todo: move to raycast system
        raycasterResult->value = raycasterTarget->value; // || windowRaycasted->value;
        // released
        if (click_type == 0) {
            continue;
        } else if (click_type == 1) {
            clickingEntity->value = raycasterTarget->value; // clicked
            on_element_clicked(world, player, clickingEntity->value);
        }
        if (click_type == 1) { // clicked
            /*if (windowRaycasted->value != windowTarget->value) {
                windowTarget->value = windowRaycasted->value;
                // now set WindowToTop entity to windowTarget->value
                // then next in canvas system, reset windows layers to top of window stack
                zox_set(canvas, WindowToTop, { windowTarget->value });
            }*/
            if (raycasterTarget->value && zox_has(raycasterTarget->value, Dragable)) {
                byte drag_mode = zox_drag_mode_none;
                if (device_mode == zox_device_mode_keyboardmouse) {
                    drag_mode = zox_drag_mode_mouse;
                } else if (device_mode == zox_device_mode_touchscreen) {
                    drag_mode = zox_drag_mode_finger;
                }
                set_element_dragged(world, player, raycasterTarget->value, drag_mode);
            }
        } else if (click_type == 2) { // released
            if (raycasterTarget->value == clickingEntity->value) {
                on_element_released(world, player, raycasterTarget->value);
            }
            clickingEntity->value = 0;
        }
    }
} zox_sys_end(ZeviceClickSystem);

// TODO: When UI clicked, set window to clicked!
/*zox_sys2(WindowClickSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceLink);
    zox_sys_in(RaycasterTarget);
    zox_sys_in(WindowRaycasted);
    zox_sys_out(RaycasterResult);
    zox_sys_out(ClickingEntity);
    zox_sys_out(WindowTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceLink, device);
        zox_sys_i(RaycasterTarget, raycasterTarget);
        zox_sys_i(WindowRaycasted, windowRaycasted);
        zox_sys_o(RaycasterResult, raycasterResult);
        zox_sys_o(ClickingEntity, clickingEntity);
        zox_sys_o(WindowTarget, windowTarget);
        if (!zox_valid(device->value)) {
            zox_log_error(" device null from zevice [%lu]", it->entities[i])
            continue;
        }
        if (zox_gett_value(device->value, DeviceDisabled)) {
            continue;
        }
        entity player = zox_get_value(device->value, PlayerLink)
        if (!player) {
            // zox_log(" ! device has null player [%lu]\n", device)
            continue;
        }
        byte device_mode = zox_get_value(player, DeviceMode);
        entity canvas = zox_get_value(player, CanvasLink)
        byte click_type = 0;
        if (zox_has(e, ZevicePointer)) {
            byte click_value = zox_get_value(e, ZevicePointer);

            if (devices_get_pressed_this_frame(click_value)) click_type = 1;
            else if (devices_get_released_this_frame(click_value)) click_type = 2;
        }

        if (zox_has(e, ZeviceButton)) {
            zox_geter(e, DeviceButtonType, deviceButtonType);

            if (deviceButtonType->value == zox_device_button_a) {
                zox_geter(e, ZeviceDisabled, zeviceDisabled);

                if (!zeviceDisabled->value) {
                    byte click_value = zox_get_value(e, ZeviceButton);

                    if (devices_get_pressed_this_frame(click_value)) {
                        click_type = 1;
                    } else if (devices_get_released_this_frame(click_value)) {
                        click_type = 2;
                    }
                }
            }
        }
        // used for virtual joysticks to see if a t arget was raycasted, todo: move to raycast system
        raycasterResult->value = raycasterTarget->value || windowRaycasted->value;
        // released
        if (click_type == 0) {
            continue;
        } else if (click_type == 1) {
            clickingEntity->value = raycasterTarget->value; // clicked
            on_element_clicked(world, player, clickingEntity->value);
        }
        if (click_type == 1) { // clicked
            if (windowRaycasted->value != windowTarget->value) {
                windowTarget->value = windowRaycasted->value;
                // now set WindowToTop entity to windowTarget->value
                // then next in canvas system, reset windows layers to top of window stack
                zox_set(canvas, WindowToTop, { windowTarget->value });
            }

            if (raycasterTarget->value && zox_has(raycasterTarget->value, Dragable)) {
                byte drag_mode = zox_drag_mode_none;
                if (device_mode == zox_device_mode_keyboardmouse) {
                    drag_mode = zox_drag_mode_mouse;
                } else if (device_mode == zox_device_mode_touchscreen) {
                    drag_mode = zox_drag_mode_finger;
                }
                set_element_dragged(world, player, raycasterTarget->value, drag_mode);
            }
        } else if (click_type == 2) { // released
            if (raycasterTarget->value == clickingEntity->value) {
                on_element_released(world, player, raycasterTarget->value);
            }
            clickingEntity->value = 0;
        }
    }
} zox_sys_end(ZeviceClickSystem);
*/
