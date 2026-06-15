entity spawn_player(ecs *world, entity prefab, entity game) {
    zox_instance(prefab);
    zox_name("player");
    zox_set_parent(world, e, game);
    // adds all devices created in inputs module
    if (local_keyboard) {
        zox_set_parent(world, local_keyboard, e);
    }
    if (local_mouse) {
        zox_set_parent(world, local_mouse, e);
    }
    if (local_touchscreen) {
        zox_set_parent(world, local_touchscreen, e);
    }
    entity app = zox_get_parent(world, game);
    entity devices[zox_children_capacity];
    uint length = zox_get_children_by_id(world, app, devices, zox_children_capacity, zox_id(Device));
    for (int i = 0; i < length; i++) {
        entity device = devices[i];
        zox_set_parent(world, device, e);
    }
    return e;
}
