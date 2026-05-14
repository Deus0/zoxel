// TODO: add a bunch of modifiers, like noise, eyes, etc for tests
entity spawn_model_nodegraph_slime(ecs* world, entity prefab, byte depth, byte3 size, color skin_color, color eye_color) {
    byte3 core_position = byte3_half(size);
    byte eye_ridge = byte_div(size.x, 5.6f);
    byte3 eye_size = byte3_single(byte_div(size.x, 5.6f));
    byte eye_y = byte_div(size.y, 2.2f);
    byte eye_pos_z = size.z - eye_size.z / 2;
    byte3 leye_position = (byte3) { size.x / 2 - eye_size.x - eye_ridge / 2, eye_y, eye_pos_z };
    byte3 reye_position = (byte3) { size.x / 2 + eye_ridge / 2, eye_y, eye_pos_z };
    // Nodes!!
    entity colors_node = spawn_node_model_colors(world, skin_color, 1);
    entity colors_node2 = spawn_node_model_colors(world, eye_color, 1);
    new_link_single_node(world, colors_node, colors_node2);
    entity fill_node = spawn_node_model_at(world, prefab, zox_model_node_fill, core_position, size, 1);
    zox_set(fill_node, NodeDepth, { depth });
    new_link_single_node(world, colors_node2, fill_node);
    entity left_eye_node = spawn_node_model_at(world, prefab, zox_model_node_paint, leye_position, eye_size, 2);
    new_link_single_node(world, fill_node, left_eye_node);
    entity right_eye_node = spawn_node_model_at(world, prefab, zox_model_node_paint, reye_position, eye_size, 2);
    new_link_single_node(world, left_eye_node, right_eye_node);
    return colors_node;
}
