entity spawn_blueprint_chest(
    ecs* world,
    byte3 nsize,
    byte depth,
    color chest_color)
{
    // colors node
    entity e = spawn_node_model_colors(world, chest_color, 1);
    // fill node
    // first fill is a blob pillar
    byte3 nsize_1 = (byte3) {
        (4 * nsize.x) / 5,
        nsize.y,
        (4 * nsize.z) / 5,
    };
    byte3 nposition_1 = (byte3) {
        nsize.x / 2,
        nsize_1.y / 2,
        nsize.z / 2
    };
    // Upper Chest Blob: spans the shoulder joints part
    byte3 nsize_2 = (byte3) {
        nsize.x,
        1 + nsize.y / 4,
        nsize.z
    };
    byte3 nposition_2 = (byte3) {
        nsize.x / 2,
        ((7 * nsize.y) / 8) - nsize_2.y / 2,
        nsize.z / 2
    };
    // Create our nodes
    entity node_1 = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, nposition_1, nsize_1, 1);
    zox_set(node_1, NodeDepth, { depth });
    new_link_single_node(world, e, node_1);
    entity node_2 = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, nposition_2, nsize_2, 1);
    zox_set(node_2, NodeDepth, { depth });
    new_link_single_node(world, node_1, node_2);
    // Finish Him!
    entity end_node = spawn_node_model_end(world);
    new_link_single_node(world, node_2, end_node);
    return e;
}
