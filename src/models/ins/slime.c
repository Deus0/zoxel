

entity spawn_model_nodegraph_slime(
    ecs* world,
    entity parent)
{
    byte3 size = get_scaled_size(nodegraph_max_depth, float3_one);
    entity prefab = prefab_node_model;
    float eye_spacing = 5.6f;
    float eye_height = 2.2f;
    byte3 core_position = byte3_half(size);
    byte eye_ridge = byte_div(size.x, eye_spacing);
    byte3 eye_size = byte3_single(byte_div(size.x, eye_spacing));
    byte eye_y = byte_div(size.y, eye_height);
    byte eye_pos_z = size.z - eye_size.z / 2;
    byte3 leye_position = (byte3) { size.x / 2 - eye_size.x - eye_ridge / 2, eye_y, eye_pos_z };
    byte3 reye_position = (byte3) { size.x / 2 + eye_ridge / 2, eye_y, eye_pos_z };
    // Nodes!!
    // Set the size
    float3 size_min = float3_single(0.52f);
    float3 size_max = float3_single(0.9f);
    entity root_node = spawn_node_model_size(world, size_min, size_max);
    // Add our colors
    entity colors_node1 = spawn_node_model_colors_seed(world, 1);
    new_link_single_node(world, root_node, colors_node1);
    entity colors_node2 = spawn_node_model_colors_seed(world, 1);
    new_link_single_node(world, colors_node1, colors_node2);
    entity fill_node = spawn_node_model_at(world, prefab, zox_model_node_fill, core_position, size, 1);
    new_link_single_node(world, colors_node2, fill_node);
    entity left_eye_node = spawn_node_model_at(world, prefab, zox_model_node_paint, leye_position, eye_size, 2);
    new_link_single_node(world, fill_node, left_eye_node);
    entity right_eye_node = spawn_node_model_at(world, prefab, zox_model_node_paint, reye_position, eye_size, 2);
    new_link_single_node(world, left_eye_node, right_eye_node);
    // Everything good must come to an end
    entity end_node = spawn_node_model_end(world);
    new_link_single_node(world, right_eye_node, end_node);
    zox_set_parent(world, root_node, parent);
    return root_node;
}
