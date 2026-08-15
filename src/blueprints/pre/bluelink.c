entity spawn_prefab_bluelink(ecs *world, entity prefab) {
    zox_instance(prefab);
    zox_prefab_name("bluelink");
    zox_add(e, Bluelink);
    return e;
}

entity spawn_bluelink(ecs *world, entity prefab, entity e1, entity e2) {
    entity e = spawn_node_link(world, prefab, e1, e2);
    zox_name("bluelink");
    return e;
}
