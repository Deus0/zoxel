// todo: reuse parts of ZeviceClickSystem in this
// this is now from zevice
zox_sys2(DeviceClickSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceDisabled);
    zox_sys_in(PlayerLink);
    zox_sys_in(RaycasterTarget);
    zox_sys_in(WindowRaycasted);
    zox_sys_in(Children);
    zox_sys_out(ClickingEntity);
    zox_sys_out(WindowTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(DeviceDisabled, disabled);
        zox_sys_i(PlayerLink, playerLink);
        zox_sys_i(RaycasterTarget, raycasterTarget);
        zox_sys_i(WindowRaycasted, windowRaycasted);
        zox_sys_i(Children, children);
        zox_sys_o(ClickingEntity, clickingEntity);
        zox_sys_o(WindowTarget, windowTarget);

        if (disabled->value) {
            continue;
        }

        entity player = playerLink->value;
        if (!zox_valid(player) || !zox_has(player, CanvasLink)) {
            continue;
        }

        entity canvas = zox_get_value(player, CanvasLink);
        if (!zox_valid(canvas)) {
            continue;
        }

        byte input_type = 0;
        for (byte j = 0; j < children->length; j++) {
            entity zevice = children->value[j];

            if (!zox_valid(zevice)) {
                continue;
            }

            if (!zox_has(zevice, ZeviceButton)) continue;
            if (!zox_has(zevice, DeviceButtonType)) continue;

            byte button_type = zox_get_value(zevice, DeviceButtonType);

            if (button_type == zox_device_button_a) {
                byte disabled = zox_get_value(zevice, ZeviceDisabled);
                if (!disabled) {
                    zox_geter_value(zevice, ZeviceButton, byte, value);

                    if (devices_get_pressed_this_frame(value)) {
                        input_type = 1;
                    } else if (devices_get_released_this_frame(value)) {
                        input_type = 2;
                    }
                }
            }
        }

        if (!input_type) {
            continue;
        }

        zox_geter_value(player, DeviceMode, byte, dmode);
        // used for virtual joysticks to see if a t arget was raycasted, todo: move to raycast system
        // raycasterResult->value = raycasterTarget->value || windowRaycasted->value;

        // released
        if (input_type == 1) {
            clickingEntity->value = raycasterTarget->value; // clicked
            on_element_clicked(world, player, clickingEntity->value);
        }

        if (input_type == 1) { // clicked
            if (windowRaycasted->value != windowTarget->value) {
                windowTarget->value = windowRaycasted->value;
                // zox_log(" > player window target [%lu]\n", windowTarget->value)
                // todo: set it's window to top
                // now set WindowToTop entity to windowTarget->value
                // then next in canvas system, reset windows layers to top of window stack
                zox_set(canvas, WindowToTop, { windowTarget->value })
            }

            if (zox_valid(raycasterTarget->value) && zox_has(raycasterTarget->value, Dragable)) {
                byte drag_mode = zox_drag_mode_none;

                if (dmode == zox_device_mode_keyboardmouse) {
                    drag_mode = zox_drag_mode_mouse;
                } else if (dmode == zox_device_mode_touchscreen) {
                    drag_mode = zox_drag_mode_finger;
                }

                set_element_dragged(world, player, raycasterTarget->value, drag_mode);
            }
        } else if (input_type == 2) { // released
            if (raycasterTarget->value == clickingEntity->value) {
                on_element_released(world, player, raycasterTarget->value);
            }
            clickingEntity->value = 0;
        }
    }
} zox_sys_end(DeviceClickSystem);
