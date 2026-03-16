

entity spawn_blueprint_head(ecs* world, byte3 nsize) {

    color skin_color = (color) { 230, 155, 133, 255 };

    entity e = spawn_node_model_colors(world, skin_color, 1);

    byte3 nposition_1 = byte3_half(nsize);

    entity e2 = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, nposition_1, nsize, 1);
    new_link_single_node(world, e, e2);

    // spawn eyes

    byte eye_place_type = zox_model_node_paint; // fill | paint;

    byte eye_ridge = nsize.x / 6;
    if (eye_ridge < 0) eye_ridge = 1;
    byte eye_size = nsize.x / 5;
    if (eye_size < 0) eye_size = 1;
    byte half_eye_size = eye_size / 2;
    if (half_eye_size == 0) half_eye_size = 1;

    byte eye_pos_y = 1 + nsize.y / 2;
    byte eye_pos_z = nsize.z - half_eye_size;

    // eye_size++;
    if (eye_place_type == zox_model_node_paint) {
        eye_pos_z -= half_eye_size;
    }

    byte3 leye_position = (byte3) {
        nsize.x / 2 - half_eye_size - eye_ridge,
        eye_pos_y,
        eye_pos_z
    };
    byte3 reye_position = (byte3) {
        nsize.x / 2 + half_eye_size + eye_ridge,
        eye_pos_y,
        eye_pos_z
    };
    //  + nsize.x % 2
    if (eye_place_type == zox_model_node_paint) {
        // adjust for centered
        // leye_position.x += half_eye_size;
        reye_position.x -= half_eye_size;
    }

    zox_log("=> eye - ridge [%i] size [%i] at [(L:%ixR:%i)x%ix%i] nsize[%ix%ix%i]", eye_ridge, eye_size, leye_position.x, reye_position.x, eye_pos_y, eye_pos_z, nsize.x, nsize.y, nsize.z);

    color eye_color = (color) { 55, 200, 99, 255 };
    entity e3 = spawn_node_model_colors(world, eye_color, 1);
    new_link_single_node(world, e2, e3);

    entity e4 = spawn_node_model_at(world, prefab_node_model, eye_place_type, leye_position, byte3_single(eye_size), 2);
    new_link_single_node(world, e3, e4);

    entity e5 = spawn_node_model_at(world, prefab_node_model, eye_place_type, reye_position, byte3_single(eye_size), 2);
    new_link_single_node(world, e4, e5);

    return e;
}

entity spawn_blueprint_chest(ecs* world, byte3 nsize) {
    // colors node
    color mcolor = (color) { 200, 155, 133, 255 };
    entity e = spawn_node_model_colors(world, mcolor, 1);

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
    new_link_single_node(world, e, node_1);

    entity node_2 = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, nposition_2, nsize_2, 1);
    new_link_single_node(world, node_1, node_2);

    return e;
}
