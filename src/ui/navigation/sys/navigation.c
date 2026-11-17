// the grid a navigated element's in (a datagrid's icon: icon, frame, grid), its columns, 0 if none
static inline byte navigation_grid_columns(ecs* world, entity e) {
    for (int depth = 0; depth < 3 && zox_valid(e); depth++) {
        e = zox_get_parent(world, e);
        if (zox_valid(e) && zox_has(e, GridSize)) {
            return zox_getv(e, GridSize).x;
        }
    }
    return 0;
}

// the stick steps the selection through its window's NavigationElements: up and down one, in a grid (a datagrid's
//  icons) left and right one and up and down a row (its columns); a stick's x as SDL has it, right is -x
// todo: fix this, EntityTarget moved to zevices
void element_navigation_system(iter* it) {
    zox_sys_on_begin();
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
            const Keyboard* keyboard = zox_get(e2, Keyboard);
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
        // in a grid the stick's x steps too (elsewhere it's the selection's own: a slider's)
        const byte columns = navigation_grid_columns(world, current->value);
        const byte sideways = columns && float_abs(left_stick.x) > float_abs(left_stick.y);
        const float amount = sideways ? float_abs(left_stick.x) : float_abs(left_stick.y);
        // If no input
        if (amount <= restore_joystick_cutoff) {
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
        else if (amount < ui_navigation_joystick_cutoff) {
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
        int direction = 0;
        if (sideways) {
            direction = left_stick.x < 0 ? 1 : -1;
        } else if (left_stick.y >= ui_navigation_joystick_cutoff) {
            direction = columns ? -columns : -1;
        } else if (left_stick.y <= -ui_navigation_joystick_cutoff) {
            direction = columns ? columns : 1;
        }
        if (!direction) {
            continue;
        }
        // TODO: Get closest uis and their directions for directional navigation
        // This is just a quick fix
        const int next_index = (int) selected_index + direction;
        if (next_index < 0 || next_index >= (int) children_length) {
            continue;
        }
        selected_index = (sbyte) next_index;
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
            scroll_list_to_child(world, target);    // a list's hidden element scrolled into view (2D and 3D)
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
    zox_sys_on_end();
} zoxd_system(element_navigation_system);
