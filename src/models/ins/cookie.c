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
    // Dough color
    entity dough_color = spawn_node_model_colors_seed(world, 1);
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
    // Chocolate chip color
    entity chocolate_color = spawn_node_model_colors_seed(world, 2);
    new_link_single_node(
        world,
        cookie_fill,
        chocolate_color);
    // A few authored chip positions for now
    byte chip_size_value = size.x / 8;
    byte3 chip_size = byte3_single(chip_size_value);
    entity chip1 = spawn_node_model_at(
        world,
        prefab,
        zox_model_node_paint,
        (byte3) {
            size.x / 3,
            size.y,
            size.z / 3
        },
        chip_size,
        2);
    new_link_single_node(world, chocolate_color, chip1);
    entity chip2 = spawn_node_model_at(
        world,
        prefab,
        zox_model_node_paint,
        (byte3) {
            size.x * 2 / 3,
            size.y,
            size.z / 2
        },
        chip_size,
        2);

    new_link_single_node(world, chip1, chip2);
    entity chip3 = spawn_node_model_at(
        world,
        prefab,
        zox_model_node_paint,
        (byte3) {
            size.x / 2,
            size.y,
            size.z * 2 / 3
        },
        chip_size,
        2);
    new_link_single_node(world, chip2, chip3);
    entity end_node = spawn_node_model_end(world);
    new_link_single_node(world, chip3, end_node);
    zox_set_parent(world, root_node, parent);
    return root_node;
}
