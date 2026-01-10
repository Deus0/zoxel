// todo: fix this, RaycasterTarget moved to zevices
zox_sys2(ElementNavigationSystem) {
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceLinks);
    zox_sys_in(DeviceMode);
    zox_sys_out(NavigatorState);
    zox_sys_out(NavigatorTimer);
    zox_sys_out(RaycasterTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceLinks, devices);
        zox_sys_i(DeviceMode, dmode);
        zox_sys_o(RaycasterTarget, current);
        zox_sys_o(NavigatorState, state);
        zox_sys_o(NavigatorTimer, timer);

        // Navigation needs a current selection
        if (!zox_valid(current->value)) {
            continue;
        }

        byte device_mode = dmode->value;

        if (device_mode != zox_device_mode_gamepad &&
            device_mode != zox_device_mode_keyboardmouse
        ) {
            if (!state->value) {
                state->value = 1;
                timer->value = 0;
            }
            continue;
        }

        // Get Input for Navigation
        float2 left_stick = float2_zero;
        for (int j = 0; j < devices->length; j++) {
            const entity device = devices->value[j];

            if (!zox_valid(device) || zox_gett_value(device, DeviceDisabled)) {
                continue;
            }

            if (zox_has(device, Keyboard)) {
                zox_geter(device, Keyboard, keyboard)
                if (keyboard->down.pressed_this_frame) {
                    left_stick.y -= 1;
                } else if (keyboard->up.pressed_this_frame) {
                    left_stick.y += 1;
                } else  if (keyboard->left.pressed_this_frame) {
                    left_stick.x -= 1;
                } else if (keyboard->right.pressed_this_frame) {
                    left_stick.x += 1;
                }
            } else if (zox_has(device, Gamepad)) {
                zox_geter(device, Children, zevices);

                for (int k = 0; k < zevices->length; k++) {
                    const entity zevice = zevices->value[k];

                    if (!zox_valid(zevice) || !zox_has(zevice, ZeviceStick)) {
                        continue;
                    }


                    zox_geter(zevice, ZeviceDisabled, disabled);
                    if (disabled->value) {
                        continue;
                    }
                    zox_geter(zevice, ZeviceStick, stick);
                    left_stick.x += stick->value.x;
                    left_stick.y += stick->value.y;
                }
            }
        }

        // If no input
        if (float_abs(left_stick.y) < restore_joystick_cutoff) {
            state->value = 0;
            continue;
        }

        // if stops input
        if (state->value == 1) {
            // here we are waiting for gamepad to stop moving, as didn't start within gamepad mode
            continue;
        } else if (float_abs(left_stick.y) < ui_navigation_joystick_cutoff) {
            timer->value = 0;
            continue;
        } else if (timer->value > 0) {
            timer->value -= delta_time;
            if (timer->value < 0) {
                timer->value = 0;
            }
            continue;
        }

        // using selected window, we navigation elements of that... this could be done better
        zox_geter_value(current->value, ParentLink, entity, parent);
        zox_geter(parent, Children, children);

        unsigned did_find = 0;
        for (int k = 0; k < children->length; k++) {
            const entity child = children->value[k];

            if (!zox_valid(child) || child != current->value || !zox_has(child, Selectable)) {
                continue;
            }

            did_find = 1;
            if (left_stick.y >= ui_navigation_joystick_cutoff && k > 1) {
                raycaster_select_element(world, e, children->value[k - 1]);
            } else if (left_stick.y <= -ui_navigation_joystick_cutoff && k != children->length - 1) {
                raycaster_select_element(world, e, children->value[k + 1]);
            }
            break;
        }
        if (did_find) {
            if (timer->value < -ui_navigation_timing / 2) {
                timer->value = ui_navigation_timing;
            } else {
                timer->value += ui_navigation_timing;
            }
        }
    }
} zox_sys_end(ElementNavigationSystem);