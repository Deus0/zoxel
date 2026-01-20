entity spawn_node_model(ecs* world, entity p, byte type) {
    zox_instance(p);
    zox_name("node_model");
    if (type) {
        zox_set(e, NodeType, { type });
    }
    return e;
}