entity spawn_model_nodegraph_cookie(
    ecs* world,
    entity parent)
{
    byte3 size = get_scaled_size(nodegraph_max_depth, float3_one);
    entity prefab = prefab_node_model;
    byte3 center = byte3_half(size);
    // Set random cookie size
    float3 size_min = (float3) { 0.55f, 0.55f, 0.18f };
    float3 size_max = (float3) { 0.95f, 0.95f, 0.32f };
    entity root_node = spawn_node_model_size(
        world,
        size_min,
        size_max);
    zox_add(root_node, SameXY);
    // dough: seeded, an alien world's (realm colour themes later, todo.md), two shades (crumbs the second)
    entity dough_color = spawn_node_model_colors_seed(world, 2);
    new_link_single_node(world, root_node, dough_color);
    // Cookie body
    entity cookie_fill = spawn_node_model_at(
        world,
        prefab,
        zox_model_node_fill,
        center,
        size,
        1);
    new_link_single_node(world, dough_color, cookie_fill);
    // a bite out of its edge, through it (voxel 0 clears)
    entity bite = spawn_node_model_at(world, prefab, zox_model_node_fill,
        (byte3) { size.x * 13 / 16, size.y * 13 / 16, center.z }, (byte3) { size.x / 3, size.y / 3, size.z }, 0);
    new_link_single_node(world, cookie_fill, bite);
    // chips: the dough's complement (HueComplement, seeded), two shades of it
    entity chocolate_color = spawn_node_model_colors_seed(world, 2);
    zox_add(chocolate_color, HueComplement);
    new_link_single_node(
        world,
        bite,
        chocolate_color);
    // chips: spots of both shades in its faces (speckle), then crumbs of the dough's other shade on it (sprinkle)
    //  (counts are the most, each cookie rolls its own, positions from its seed)
    const byte3 everywhere = byte3_zero;
    entity chips_dark = spawn_node_model_sprinkle(world, zox_model_node_speckle, 3, 5, 4, everywhere, size);
    new_link_single_node(world, chocolate_color, chips_dark);
    entity chips_light = spawn_node_model_sprinkle(world, zox_model_node_speckle, 4, 5, 4, everywhere, size);
    new_link_single_node(world, chips_dark, chips_light);
    entity crumbs = spawn_node_model_sprinkle(world, zox_model_node_sprinkle, 2, 20, 0, everywhere, size);
    new_link_single_node(world, chips_light, crumbs);
    entity end_node = spawn_node_model_end(world);
    new_link_single_node(world, crumbs, end_node);
    zox_set_parent(world, root_node, parent);
    return root_node;
}
