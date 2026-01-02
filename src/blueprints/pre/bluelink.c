entity spawn_prefab_bluelink(ecs *world, const entity prefab) {
    zox_instance(prefab)
    zox_prefab_name("bluelink")
    zox_add_tag(e, Bluelink)
    return e;
}

entity spawn_bluelink(ecs *world, const entity prefab, const entity e1, const entity e2) {
    const entity e = spawn_node_link(world, prefab, e1, e2);
    zox_name("bluelink")
    return e;
}
