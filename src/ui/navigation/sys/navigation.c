// todo: fix this, RaycasterTarget moved to zevices
zox_sys2(ElementNavigationSystem) {
    byte dbg_log = 0;
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceMode);
    zox_sys_out(NavigatorState);
    zox_sys_out(NavigatorTimer);
    zox_sys_out(RaycasterTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
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
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (!zox_valid(e2) || zox_gett_value(e2, DeviceDisabled)) {
                continue;
            }
            uint children_capacity = zox_children_capacity;
            entity children[children_capacity];
            uint children_length = zox_get_children(world, e2, children, children_capacity);
            for (uint k = 0; k < children_length; k++) {
                entity e3 = children[k];
                if (!zox_valid(e3)) {
                    continue;
                }
                zox_geter_value(e3, ZeviceDisabled, byte, disabled);
                if (disabled) {
                    continue;
                }
                if (zox_has(e3, ZeviceStick)) {
                    zox_geter(e3, ZeviceStick, stick);
                    left_stick.x += stick->value.x;
                    left_stick.y += stick->value.y;
                }
            }
            if (zox_has(e2, Keyboard)) {
                zox_geter(e2, Keyboard, keyboard);
                if (keyboard->down.is_pressed) {
                    left_stick.y -= 1;
                } else if (keyboard->up.is_pressed) {
                    left_stick.y += 1;
                } else if (keyboard->left.is_pressed) {
                    left_stick.x -= 1;
                } else if (keyboard->right.is_pressed) {
                    left_stick.x += 1;
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
        entity window = zox_get_parent_by_id(world, current->value, zox_id(Window));
        // entity parent = zox_get_parent(world, current->value);
        entity children[zox_children_capacity];
        // uint children_length = 0;
        // uint children_length = zox_get_children_by_id(world, parent, children, layouts2_children_capacity, zox_id(Selectable));
        uint children_length = zox_get_children_by_id_recursive(world, window, children, zox_children_capacity, zox_id(NavigationElement), 0);
        /*iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count && children_length < zox_children_capacity; j++) {
                entity e2 = it2.entities[j];
                if (zox_valid(e2) && zox_has(e2, Button) && zox_has(e2, Header)) {
                    children[children_length] = e2;
                    children_length++;
                }
            }
        }*/
        for (byte k = 0; k < children_length; k++) {
            entity child = children[k];
            if (child == current->value) {
                selected_index = k;
                break;
            }
        }
        if (selected_index == -1) {
            zox_loge("Could not find child index of navigated one [%s]", zox_get_name(current->value));
            // continue;
            selected_index = 0;
        }
        // zox_log("Going Down Town [%i] -> %f", selected_index, left_stick.y);
        entity target = 0;
        if (left_stick.y >= ui_navigation_joystick_cutoff && selected_index >= 1) {
            target = children[selected_index - 1];
        } else if (left_stick.y <= -ui_navigation_joystick_cutoff && selected_index < children_length - 1) {
            target = children[selected_index + 1];
        }
        if (target && target != current->value) {
            raycaster_select_element(world, e, target);
            if (timer->value < -ui_navigation_timing / 2) {
                timer->value = ui_navigation_timing;
            } else {
                timer->value += ui_navigation_timing;
            }
            if (dbg_log) {
                zox_log("New Navigation Target UI [%s]", zox_get_name(target));
            }
        }
    }
} zox_sys_end(ElementNavigationSystem);
