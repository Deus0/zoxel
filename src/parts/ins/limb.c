entity spawn_blueprint_flesh_part(
    ecs* world,
    byte3 nsize,
    byte depth,
    color skin_color)
{
    skin_color.r += rand_range(-15, 15);
    skin_color.g += rand_range(-15, 15);
    skin_color.b += rand_range(-15, 15);
    entity e = spawn_node_model_colors(
        world,
        skin_color,
        1);
    byte3 nposition_1 = byte3_half(nsize);
    entity e2 = spawn_node_model_at(
        world,
        prefab_node_model,
        zox_model_node_fill,
        nposition_1,
        nsize,
        1);
    zox_setv(e2, NodeDepth, depth);
    new_link_single_node(world, e, e2);
    // Finish Him!
    entity end_node = spawn_node_model_end(world);
    new_link_single_node(world, e2, end_node);
    return e;
}
