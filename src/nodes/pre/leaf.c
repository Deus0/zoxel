entity spawn_prefab_node_leaf(ecs *world) {
    zox_prefab();
    zox_prefab_name("node");
    zox_add_tag(e, Node);
    zox_add_tag(e, Nodeleaf);
    zox_prefab_set(e, NodeLinks, { 0 });
    return e;
}

entity spawn_node_leaf(ecs *world, const entity prefab) {
    zox_instance(prefab);
    zox_name("node");
    return e;
}