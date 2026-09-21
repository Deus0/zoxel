entity spawn_prefab_node(ecs *world) {
    zox_prefab();
    zox_prefab_name("node");
    zox_add(e, Node);
    zox_add(e, Nodeleaf);
    return e;
}

entity spawn_node_leaf(ecs *world, entity p) {
    zox_instance(p);
    zox_name("node");
    return e;
}
