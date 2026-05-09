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

        if (dmode->value != zox_device_mode_gamepad && !(keyboard_navigation_mode && dmode->value == zox_device_mode_keyboardmouse)
        ) {
            if (!state->value) {
                state->value = 1;
                timer->value = 0;
            }
            continue;
        }

        // Get Input for Navigation
        float2 left_stick = float2_zero;
        for (byte j = 0; j < devices->length; j++) {
            entity device = devices->value[j];

            if (!zox_valid(device) || zox_gett_value(device, DeviceDisabled)) {
                continue;
            }

            if (zox_has(device, Keyboard)) {
                zox_geter(device, Keyboard, keyboard);

                if (keyboard->down.is_pressed) {
                    left_stick.y -= 1;
                } else if (keyboard->up.is_pressed) {
                    left_stick.y += 1;
                } else if (keyboard->left.is_pressed) {
                    left_stick.x -= 1;
                } else if (keyboard->right.is_pressed) {
                    left_stick.x += 1;
                }

            } else if (zox_has(device, Gamepad)) {
                zox_geter(device, Children, zevices);

                for (byte k = 0; k < zevices->length; k++) {
                    entity zevice = zevices->value[k];

                    if (!zox_valid(zevice) || !zox_has(zevice, ZeviceStick)) {
                        continue;
                    }

                    zox_geter_value(zevice, ZeviceDisabled, byte, zdisabled);
                    if (zdisabled) {
                        continue;
                    }

                    zox_geter(zevice, ZeviceStick, stick);

                    left_stick.x += stick->value.x;
                    left_stick.y += stick->value.y;
                }
            }
        }

        // If no input
        if (float_abs(left_stick.y) <= restore_joystick_cutoff) {
            timer->value = 0;
            if (state->value) {
                state->value = 0;
                zox_log("Released Navigation");
            }
            continue;
        }

        // if stops input
        if (state->value) {
            // zox_log("Going Down Town %f : %i", left_stick.y, state->value);
            // here we are waiting for gamepad to stop moving, as didn't start within gamepad mode
            continue;
        }
        // If no input, reset timer
        else if (float_abs(left_stick.y) < ui_navigation_joystick_cutoff) {
            timer->value = 0;
            continue;
        }
        // If timer still cooling down, continue
        else if (timer->value > 0) {
            timer->value -= delta_time;
            if (timer->value < 0) {
                timer->value = 0;
            }
            continue;
        }

        // using selected window, we navigation elements of that... this could be done better

        // TODO: Move up to window, grab all navigation elements, then find one below?

        // Get Selected Index TODO: Make this a generic parent function
        sbyte selected_index = -1;
        entity parent = zox_get_parent(world, current->value);
        entity children[layouts2_children_capacity];
        uint children_length = zox_get_children(world, parent, children, layouts2_children_capacity);
        for (byte k = 0; k < children_length; k++) {
            entity child = children[k];
            if (!zox_valid(child) || !zox_has(child, Selectable)) {
                continue;
            }
            if (child == current->value) {
                selected_index = k;
                break;
            }
        }
        if (selected_index != -1) {
            // zox_log("Going Down Town [%i] -> %f", selected_index, left_stick.y);
            entity target = 0;
            if (left_stick.y >= ui_navigation_joystick_cutoff
                && selected_index >= 1) {
                target = children[selected_index - 1];
            } else if (left_stick.y <= -ui_navigation_joystick_cutoff
                && selected_index < children_length - 1) {
                target = children[selected_index + 1];
            }
            if (target) {
                raycaster_select_element(world, e, target);

                if (timer->value < -ui_navigation_timing / 2) {
                    timer->value = ui_navigation_timing;
                } else {
                    timer->value += ui_navigation_timing;
                }
            }
        }
    }
} zox_sys_end(ElementNavigationSystem);
