entity spawn_blueprint_hips(ecs* world, byte3 nsize, byte depth) {
    color skin_color = (color) { 230, 155, 133, 255 };
    entity e = spawn_node_model_colors(world, skin_color, 1);
    byte3 nposition_1 = byte3_half(nsize);
    entity e2 = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, nposition_1, nsize, 1);
    zox_set(e2, NodeDepth, { depth });
    new_link_single_node(world, e, e2);
    return e;
}

entity spawn_blueprint_shoulder(ecs* world, byte3 nsize, byte depth) {
    color skin_color = (color) { 230, 155, 133, 255 };
    entity e = spawn_node_model_colors(world, skin_color, 1);
    byte3 nposition_1 = byte3_half(nsize);
    entity e2 = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, nposition_1, nsize, 1);
    zox_set(e2, NodeDepth, { depth });
    new_link_single_node(world, e, e2);
    return e;
}
