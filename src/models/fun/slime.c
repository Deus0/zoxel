// TODO: add a bunch of modifiers, like noise, eyes, etc for tests
entity spawn_model_nodegraph_slime(ecs* world, entity p, byte3 size) {

    // byte vlength = powers_of_two[depth];

    // byte3 core_position = byte3_single(16);
    // byte3 core_size = (byte3) { vlength, vlength / 2, vlength };
    byte3 core_position = byte3_half(size);

    byte eye_ridge = 1 + size.x / 6;
    byte3 eye_size = byte3_single(1 + size.x / 6);
    byte eye_y = size.y / 2 + 2;
    byte eye_pos_z = size.z - eye_size.z / 2;
    byte3 leye_position = (byte3) { size.x / 2 - eye_size.x - eye_ridge / 2, eye_y, eye_pos_z };
    byte3 reye_position = (byte3) { size.x / 2 + eye_ridge / 2, eye_y, eye_pos_z };

    // entity e = spawn_node_model(world, p, zox_model_node_colors);
    color mcolor = (color) { 55, 255, 255, 255 };
    entity e = spawn_node_model_colors(world, mcolor, 1);

    color mcolor2 = (color) { 255, 55, 55, 255 };
    entity e1 = spawn_node_model_colors(world, mcolor2, 1);
    new_link_single_node(world, e, e1);

    entity e2 = spawn_node_model_at(world, p, zox_model_node_fill, core_position, size, 1);
    new_link_single_node(world, e1, e2);


    entity e3 = spawn_node_model_at(world, p, zox_model_node_paint, leye_position, eye_size, 2);
    new_link_single_node(world, e2, e3);

    entity e4 = spawn_node_model_at(world, p, zox_model_node_paint, reye_position, eye_size, 2);
    new_link_single_node(world, e3, e4);

    return e;
}
