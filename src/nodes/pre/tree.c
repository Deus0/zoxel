entity spawn_prefab_node_tree(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("nodetree");
    zox_add_tag(e, Node);
    zox_add_tag(e, Nodetree);
    return e;
}

entity spawn_node_tree(ecs *world, const entity prefab) {
    zox_instance(prefab);
    zox_name("nodetree");
    return e;
}

