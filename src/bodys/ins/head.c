entity spawn_blueprint_head(ecs* world, byte3 nsize, byte depth, color skin_color, color eye_color) {
    entity e = spawn_node_model_colors(world, skin_color, 1);
    byte3 nposition_1 = byte3_half(nsize);
    entity e2 = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, nposition_1, nsize, 1);
    zox_set(e2, NodeDepth, { depth });
    new_link_single_node(world, e, e2);
    // spawn eyes
    byte eye_place_type = zox_model_node_paint; // fill | paint;
    byte eye_ridge = nsize.x / 8;
    byte eye_size = nsize.x / 7;
    byte half_eye_size = eye_size / 2;
    byte eye_pos_y = int_ceilf(nsize.y / 1.9f);
    if (eye_ridge == 0) {
        eye_ridge = 1;
    }
    if (eye_size == 0) {
        eye_size = 1;
    }
    if (half_eye_size == 0) {
        half_eye_size = 1;
    }
    byte eye_pos_z = nsize.z - half_eye_size;
    // eye_size++;
    if (eye_place_type == zox_model_node_paint) {
        eye_pos_z -= half_eye_size;
    }
    byte3 leye_position = (byte3) {
        int_ceilf(nsize.x / 2.01f) - eye_size + 1 - eye_ridge,
        eye_pos_y,
        eye_pos_z
    };
    byte3 reye_position = (byte3) {
        int_floorf(nsize.x / 2.01f) + eye_ridge,
        eye_pos_y,
        eye_pos_z
    };
    zox_log("Eye size[%i] L[%i] R[%i] out of [%i]... Ridge [%i]", eye_size, leye_position.x, reye_position.x, nsize.x, eye_ridge);
    /*if (eye_place_type == zox_model_node_paint) {
        // adjust for centered
        leye_position.x += half_eye_size;
        reye_position.x -= half_eye_size;
    }*/
    entity e3 = spawn_node_model_colors(world, eye_color, 1);
    new_link_single_node(world, e2, e3);
    entity e4 = spawn_node_model_at(world, prefab_node_model, eye_place_type, leye_position, byte3_single(eye_size), 2);
    new_link_single_node(world, e3, e4);
    entity e5 = spawn_node_model_at(world, prefab_node_model, eye_place_type, reye_position, byte3_single(eye_size), 2);
    new_link_single_node(world, e4, e5);
    // zox_log("=> eye - ridge [%i] size [%i] at [(L:%ixR:%i)x%ix%i] nsize[%ix%ix%i]", eye_ridge, eye_size, leye_position.x, reye_position.x, eye_pos_y, eye_pos_z, nsize.x, nsize.y, nsize.z);
    return e;
}
