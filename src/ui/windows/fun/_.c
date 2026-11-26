void raycaster_select_window_children(ecs *world, entity e, entity window) {
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (!zox_valid(e2)) {
                continue;
            }
            raycaster_select_window_children(
                world,
                e2,
                window);
        }
    }
    entity devices[zox_children_capacity];
    uint length = zox_get_children_by_id(
        world,
        e,
        devices,
        zox_children_capacity,
        zox_id(Device));
    for (uint j = 0; j < length; j++) {
        entity e2 = devices[j];
        if (!e2) {
            continue;
        }
        raycaster_select_window_children(
            world,
            e2,
            window);
    }
}

static void button_event_close_window(ecs* world, ClickEventData event) {
    const entity header = zox_get_parent(world, event.clicked);
    const entity window = zox_get_parent(world, header);
    dock_close(world, zox_get_parent(world, window), window);
}
