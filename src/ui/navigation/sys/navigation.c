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

// an element's centre on the screen (pointer space, y up), 0 if it's not a shown 2D one
static inline byte navigation_centre(ecs* world, const CanvasViewport* viewport, entity e, float2* centre) {
    if (!zox_has(e, Position2) || !zox_has(e, LayoutSize) || (zox_has(e, RenderDisabled) && zox_getv(e, RenderDisabled))) {
        return 0;
    }
    const int4 b = get_element_pointer_bounds(world, viewport, e, zox_getv(e, Position2), zox_getv(e, LayoutSize));
    *centre = (float2) { (b.x + b.y) * 0.5f, (b.z + b.w) * 0.5f };
    return 1;
}

// off its window's edge: the closest element that way in another open window of its canvas (the inventory's last
//  row down to the action bar), 0 if none; dir is on the screen, y up
static entity navigation_step_window(ecs* world, entity current, entity window, float2 dir) {
    const entity canvas = zox_get_parent_by_id(world, window, zox_id(Canvas));
    CanvasViewport viewport;
    float2 from;
    if (!zox_valid(canvas) || !get_canvas_viewport(world, canvas, &viewport) ||
        !navigation_centre(world, &viewport, current, &from)) {
        return 0;
    }
    entity best = 0;
    float best_score = 0;
    iter it = zox_children(world, canvas);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity other = it.entities[i];
            if (other == window || !zox_has(other, Window) || zox_has(other, UIClosing)) {
                continue;
            }
            entity elements[zox_children_capacity];
            const uint count = zox_get_children_by_id_recursive(world, other, elements, zox_children_capacity,
                zox_id(NavigationElement), 0);
            for (uint j = 0; j < count; j++) {
                float2 to;
                if (!navigation_centre(world, &viewport, elements[j], &to)) {
                    continue;
                }
                const float2 d = { to.x - from.x, to.y - from.y };
                const float along = d.x * dir.x + d.y * dir.y;
                if (along <= 0) {
                    continue;
                }
                // straight that way first: across counts double
                const float score = along + 2 * float_abs(d.x * dir.y - d.y * dir.x);
                if (!best || score < best_score) {
                    best = elements[j];
                    best_score = score;
                }
            }
        }
    }
    return best;
}

// an element inside a hidden part of its window (a tab not shown: its scrollview RenderDisabled), not navigated to;
//  its own RenderDisabled (scrolled out of its list) isn't, the list scrolls to it (scroll_list_to_child)
static inline byte navigation_in_hidden(ecs* world, entity e, entity window) {
    for (entity p = zox_get_parent(world, e); zox_valid(p) && p != window; p = zox_get_parent(world, p)) {
        if (zox_has(p, RenderDisabled) && zox_getv(p, RenderDisabled)) {
            return 1;
        }
    }
    return 0;
}

// the shown tab's button among a window's navigation elements, 0 if it has no tabs
static inline entity navigation_active_tab(ecs* world, const entity* children, uint count) {
    for (uint k = 0; k < count; k++) {
        if (zox_has(children[k], TabButton) && zox_getv(children[k], ActiveState)) {
            return children[k];
        }
    }
    return 0;
}

// the stick steps the selection through its window's NavigationElements: up and down one, in a grid (a datagrid's
//  icons) left and right one and up and down a row (its columns); a stick's x as SDL has it, right is -x
// todo: fix this, Target moved to zevices
void element_navigation_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(NavigatorState);
    zox_sys_out(NavigatorTimer);
    zox_sys_out(Target);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(Target, current);
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
        // a hidden tab's elements aren't there to step to
        uint shown_length = 0;
        for (uint k = 0; k < children_length; k++) {
            if (children[k] == current->value || !navigation_in_hidden(world, children[k], window)) {
                children[shown_length++] = children[k];
            }
        }
        children_length = shown_length;
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
        const entity current_parent = zox_get_parent(world, current->value);
        // a tab strip is a row (LB and RB step it: navigation_tabs_system): down from a tab, its list's first, up from
        //  there, the shown tab
        if (zox_has(current->value, TabButton) && !sideways) {
            if (direction > 0) {
                for (uint j = 0; j < children_length && !target; j++) {
                    if (!zox_has(children[j], TabButton)) {
                        target = children[j];
                    }
                }
            } else {
                target = navigation_step_window(world, current->value, window, (float2) { 0, 1 });
            }
        } else if (next_index < 0 || next_index >= (int) children_length) {
            // past its window's ends: into the next window that way
            const float2 dir = sideways ? (float2) { direction, 0 } : (float2) { 0, direction < 0 ? 1 : -1 };
            target = navigation_step_window(world, current->value, window, dir);
        } else if (zox_has(current->value, ChildIndex)) {
            // a list's (or grid's) neighbour by its index, among its siblings (its parent's: another list's, or the
            //  tab strip's, share the indexes)
            const int index = (int) zox_getv(current->value, ChildIndex) + direction;
            byte found = 0;
            for (uint j = 0; j < children_length; j++) {
                const entity e2 = children[j];
                if (e2 != current->value && zox_has(e2, ChildIndex) && zox_get_parent(world, e2) == current_parent &&
                    (int) zox_getv(e2, ChildIndex) == index) {
                    target = e2;
                    found = 1;
                    break;
                }
            }
            // its list's end: the window's next element that way, not a sibling (up into the tabs: the shown one)
            for (int j = next_index; !found && j >= 0 && j < (int) children_length; j += direction) {
                if (zox_get_parent(world, children[j]) == current_parent) {
                    continue;
                }
                found = 1;
                target = zox_has(children[j], TabButton) ? navigation_active_tab(world, children, children_length) :
                    children[j];
            }
            if (!found) {
                const float2 dir = sideways ? (float2) { direction, 0 } : (float2) { 0, direction < 0 ? 1 : -1 };
                target = navigation_step_window(world, current->value, window, dir);
            }
        } else {
            target = zox_has(children[next_index], TabButton) && direction < 0 ?
                navigation_active_tab(world, children, children_length) : children[next_index];
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
