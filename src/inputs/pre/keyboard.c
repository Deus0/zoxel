entity spawn_prefab_keyboard(ecs *world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("keyboard");
    zox_prefab_add(e, Keyboard);
    return e;
}

entity spawn_keyboard(ecs *world, entity p) {
    zox_instance(p);
    zox_name("keyboard");
    zox_set(e, Keyboard, { 0 });
    return e;
}
