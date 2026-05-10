void raycaster_select_window_children(ecs *world, entity e, entity window) {
    if (zox_has(e, WindowRaycasted)) {
        zox_set(e, WindowRaycasted, { window })
    }
    entity children[layouts2_children_capacity];
    uint count = zox_get_children(world, e, children, layouts2_children_capacity);
    for (uint i = 0; i < count; i++) {
        entity child = children[i];
        if (!zox_valid(child)) {
            continue;
        }
        raycaster_select_window_children(world, child, window);
    }
    if (zox_has(e, DeviceLinks)) {
        zox_geter(e, DeviceLinks, children);
        for (int i = 0; i < children->length; i++) {
            entity child = children->value[i];
            if (!child) {
                continue;
            }
            raycaster_select_window_children(world, child, window);
        }
    }
}

/*entity get_canvas_window(ecs* world, entity e, byte find_type) {
    if (!zox_valid(e)) {
        return 0;
    }
    entity children[layouts2_children_capacity];
    uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
    for (uint j = 0; j < children_length; j++) {
        entity child = children[j];
        if (!zox_valid(child)) { // || !zox_has(child, WindowType)) {
            continue;
        }
        zox_geter_value(child, WindowType, byte, window_type);
        if (window_type == find_type) {
            return child;
        }
    }
    return 0;
}*/
