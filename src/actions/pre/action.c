entity spawn_prefab_action(ecs *world) {
    zox_prefab();
    zox_prefab_name("action");
    zox_add_tag(e, Action);
    return e;
}

entity spawn_action(ecs *world) {
    zox_instance(prefab_action);
    zox_name("action");
    return e;
}