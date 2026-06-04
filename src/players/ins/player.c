entity spawn_player(ecs *world, const entity prefab) {
    zox_instance(prefab);
    zox_name("player");
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
    return e;
}
