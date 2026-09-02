entity find_child_with_mtag_rec(ecs* world, entity e, entity tag, entity mtag, entity mtag2) {
    if (!zox_valid(e)) {
        return 0;
    }
    entity children[layouts2_children_capacity];
    uint count = zox_get_children(world, e, children, layouts2_children_capacity);
    for (uint i = 0; i < count; i++) {
        entity e2 = children[i];
        if (!zox_valid(e2)) {
            continue;
        }
        if (zox_has_id(e2, tag) && !zox_has_id(e2, mtag2) && !zox_has_id(e2, mtag2)) {
            return e2;
        }
        entity e3 = find_child_with_mtag_rec(world, e2, tag, mtag, mtag2);
        if (e3) {
            return e3;
        }
    }
    return 0;
}

void raycaster_select_first_button(ecs *world, entity e, entity window) {
    // entity button = find_child_with_mtag_rec(world, window, zox_id(NavigationElement), zox_id(Header), zox_id(CloseButton));
    entity button = zox_get_child_by_id_recursive(world, window, zox_id(NavigationElement));
    if (!button) {
        zox_loge("Window has no NavigationElements [%s]", zox_get_name(window));
        return;
    }
    // zox_log("Navigation Beginning on Window [%s] element [%s]", zox_get_name(window), zox_get_name(button));
    raycaster_select_element(world, e, button);
}

// NOTE: This is for gamepads to select ui in window
zox_sys2(ElementNavigationBeginSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceDisabled);
    zox_sys_out(EntityTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceDisabled, disabled);
        zox_sys_o(EntityTarget, current);
        if (disabled->value || zox_valid(current->value)) {
            continue;
        }
        if (dbg_log >= 2) {
            zox_log("Navigator Seeking [%s]", zox_getn(e));
        }
        entity player = zox_get_parent(world, e);
        if (!zox_valid(player) ||
            !zox_has(player, DeviceMode)
        ) {
            zox_loge("Invalid Player on [Navigator]");
            continue;
        }
        byte device_mode = zox_getv(player, DeviceMode);
        if (keyboard_navigation_mode && device_mode == zox_device_mode_keyboardmouse) {
            device_mode = zox_device_mode_gamepad;
        }
        if (device_mode != zox_device_mode_gamepad) {
            continue;
        }
        entity canvas = zox_get_link(world, player, Canvas);
        if (!zox_valid(canvas)) {
            // zox_logw("Canvas is missing from Player");
            continue;
        }
        entity window = zox_get_child_by_id(world, canvas, zox_id(NavigationWindow));
        if (!window) {
            continue;
        }
        raycaster_select_first_button(world, e, window);
    }
} zox_sys_end(ElementNavigationBeginSystem);
