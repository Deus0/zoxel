entity spawn_node_model(ecs* world, entity p, byte type) {
    zox_instance(p);
    zox_name("node_model");
    if (type) {
        zox_set(e, NodeType, { type });
    }
    return e;
}

// Add Shape3Position and Shape3Size's
entity spawn_node_model_fill(ecs* world, entity p, byte3 position, byte3 size) {
    zox_instance(p);
    zox_name("node_model");
    zox_set(e, NodeType, { zox_model_node_fill });
    zox_set(e, Shape3Position, { position });
    zox_set(e, Shape3Size, { size });
    return e;
}