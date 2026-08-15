entity spawn_prefab_bluenode(ecs *world, const entity prefab) {
    zox_instance(prefab);
    zox_prefab_name("bluenode");
    zox_add(e, Bluenode);
    return e;
}

entity spawn_bluenode(ecs *world, const entity prefab) {
    // const entity e = spawn_node(world, prefab, e1, e2);
    zox_instance(prefab);
    zox_name("bluenode");
    return e;
}
