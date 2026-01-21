// TODO: add a bunch of modifiers, like noise, eyes, etc for tests
entity spawn_model_nodegraph_slime(ecs* world, entity p) {

    // byte3 core_position = byte3_single(16);
    byte3 core_size = (byte3) { 30, 16, 30 };
    byte3 core_position = byte3_half(core_size);
    byte eye_ridge = 6;
    byte eye_y = core_size.y / 2 + 2;
    byte3 eye_size = byte3_single(6);
    byte3 leye_position = (byte3) { 16 - eye_size.x - eye_ridge / 2, eye_y, 28 };
    byte3 reye_position = (byte3) { 16 + eye_ridge / 2, eye_y, 28 };

    entity e = spawn_node_model(world, p, zox_model_node_colors);

    entity e2 = spawn_node_model_at(world, p, zox_model_node_fill, core_position, core_size);
    new_link_single_node(world, e, e2);


    entity e3 = spawn_node_model_at(world, p, zox_model_node_paint, leye_position, eye_size);
    // zox_set(e3, ColorRGB, { color_red });
    new_link_single_node(world, e2, e3);

    entity e4 = spawn_node_model_at(world, p, zox_model_node_paint, reye_position, eye_size);
    // zox_set(e3, ColorRGB, { color_red });
    new_link_single_node(world, e3, e4);

    return e;
}