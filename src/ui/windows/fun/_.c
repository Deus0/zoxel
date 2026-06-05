void raycaster_select_window_children(ecs *world, entity e, entity window) {
    if (zox_has(e, WindowRaycasted)) {
        zox_set(e, WindowRaycasted, { window })
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (!zox_valid(e2)) {
                continue;
            }
            raycaster_select_window_children(world, e2, window);
        }
    }
    entity devices[zox_children_capacity];
    uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
    for (uint j = 0; j < length; j++) {
        entity e2 = devices[j];
        if (!e2) {
            continue;
        }
        raycaster_select_window_children(world, e2, window);
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
