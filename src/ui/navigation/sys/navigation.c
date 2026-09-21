// todo: fix this, EntityTarget moved to zevices
zox_sys2(ElementNavigationSystem) {
    byte dbg_log = 0;
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(NavigatorState);
    zox_sys_out(NavigatorTimer);
    zox_sys_out(EntityTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(EntityTarget, current);
        zox_sys_o(NavigatorState, state);
        zox_sys_o(NavigatorTimer, timer);
        // Navigation needs a current selection
        if (!zox_valid(current->value)) {
            continue;
        }
        entity player = zox_get_parent(world, e);
        if (!zox_valid(player) ||
            !zox_has(player, DeviceMode)
        ) {
            zox_loge("Invalid Player on [Navigator]");
            continue;
        }
        byte device_mode = zox_getv(player, DeviceMode);
        if (device_mode != zox_device_mode_gamepad &&
            !(keyboard_navigation_mode &&
            device_mode == zox_device_mode_keyboardmouse))
        {
            if (!state->value) {
                state->value = 1;
                timer->value = 0;
            }
            continue;
        }
        // Get Input for Navigation
        float2 left_stick = float2_zero;
        uint children_capacity = zox_children_capacity;
        entity zevices[children_capacity];
        uint zevices_length = zox_get_children(
            world,
            e,
            zevices,
            children_capacity);
        for (uint k = 0; k < zevices_length; k++) {
            entity e3 = zevices[k];
            if (zox_has(e3, Disabled)) {
                continue;
            }
            if (zox_has(e3, ZeviceStick)) {
                float2 stick = zox_getv(e3, ZeviceStick);
                left_stick.x += stick.x;
                left_stick.y += stick.y;
            }
        }
        /*if (zox_has(e2, Keyboard)) {
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
        }*/
        if (dbg_log >= 2) {
            zox_log("Navigator Navigating [%s] state [%i] stick [%f] timer [%f]",
                zox_getn(e),
                state->value,
                left_stick.y,
                timer->value);
        }
        if (dbg_log) {
            zox_log("Navigator Navigating [%s] state [%i] stick [%f] timer [%f]",
                zox_getn(e),
                state->value,
                left_stick.y,
                timer->value);
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
        if (dbg_log) {
            zox_log("Navigator Navigating [%s]",
                zox_getn(e));
        }
        // using selected window, we navigation elements of that... this could be done better
        // TODO: Move up to window, grab all navigation elements, then find one below?
        // Get Selected Index TODO: Make this a generic parent function
        sbyte selected_index = -1;
        entity window = zox_get_parent_by_id(
            world,
            current->value,
            zox_id(Window));
        entity children[zox_children_capacity];
        uint children_length = zox_get_children_by_id_recursive(
            world,
            window,
            children,
            zox_children_capacity,
            zox_id(NavigationElement), 0);
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
        sbyte direction = 0;
        if (left_stick.y >= ui_navigation_joystick_cutoff) {
            direction = -1;
            //target = children[selected_index - 1];
        } else if (left_stick.y <= -ui_navigation_joystick_cutoff) {
            direction = 1;
            //target = children[selected_index + 1];
        }
        if (!direction) {
            continue;
        }
        // TODO: Get closest uis and their directions for directional navigation
        // This is just a quick fix
        selected_index += direction;
        if (selected_index < 0 || selected_index >= children_length) {
            continue;
        }
        if (zox_has(current->value, ChildIndex)) {
            // NOTE: Gets neighbor if indexes are set like a list, very rigid
            for (int j = 0; j < children_length; j++) {
                entity e2 = children[j];
                if (e2 == current->value) {
                    continue;
                }
                if (!zox_has(e2, ChildIndex)) {
                    continue;
                }
                byte index = zox_getv(e2, ChildIndex);
                if (index == selected_index) {
                    target = e2;
                    break;
                }
            }
        } else {
            target = children[selected_index];
        }
        if (target && target != current->value) {
            raycaster_select_element(
                world,
                e,
                target);
            if (timer->value < -ui_navigation_timing / 2) {
                timer->value = ui_navigation_timing;
            } else {
                timer->value += ui_navigation_timing;
            }
            if (dbg_log) {
                zox_log("New Navigation Target UI [%s]",
                    zox_getn(target));
            }
        }
    }
} zox_sys_end(ElementNavigationSystem);
