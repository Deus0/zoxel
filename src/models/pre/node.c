entity spawn_prefab_node_model(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("node_model");
    zox_prefab_set(e, NodeType, { 0 });
    return e;
}