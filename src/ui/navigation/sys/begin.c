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
    entity button = find_child_with_mtag_rec(world, window, zox_id(Button), zox_id(Header), zox_id(CloseButton));
    // find_child_with_tag(window, Button, element);
    if (!button) {
        return;
    }
    zox_log("Navigation Beginning on w[%s]", zox_get_name(window), zox_get_name(button));
    raycaster_select_element(world, e, button);
}

// This system needs no selection
zox_sys2(ElementNavigationBeginSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceMode);
    zox_sys_in(CanvasLink);
    zox_sys_out(RaycasterTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceMode, dmode);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_o(RaycasterTarget, current);
        if (zox_valid(current->value)) {
            continue;
        }
        byte device_mode = dmode->value;
        if (keyboard_navigation_mode && device_mode == zox_device_mode_keyboardmouse) {
            device_mode = zox_device_mode_gamepad;
        }
        if (device_mode != zox_device_mode_gamepad) {
            continue;
        }
        if (!zox_valid(canvas->value)) {
            zox_logw("Canvas is missing from Player");
            continue;
        }
        // find_child_with_tag(canvas->value, Window, window);
        entity window = find_child_with_tag_recursive(world, canvas->value, zox_id(NavigationWindow));
        if (!window) {
            continue;
        }
        raycaster_select_first_button(world, e, window);
    }
} zox_sys_end(ElementNavigationBeginSystem);
