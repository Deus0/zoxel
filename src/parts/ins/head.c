entity spawn_blueprint_head(
    ecs* world,
    byte3 nsize,
    byte depth,
    color skin_color,
    color eye_color)
{
    entity e = spawn_node_model_colors(world, skin_color, 1);
    // a person's head (y up, the face to +z): a tall cranium, a narrower jaw lower and forward, a neck under them
    //  - the neck: from the bottom up into the jaw, a little back (tst model_shapes checks it)
    const byte3 cranium_centre = { nsize.x / 2, (31 * nsize.y) / 50, (12 * nsize.z) / 25 };
    const byte3 cranium_size = { (4 * nsize.x) / 5, (18 * nsize.y) / 25, (21 * nsize.z) / 25 };
    const byte3 jaw_centre = { nsize.x / 2, (21 * nsize.y) / 50, (11 * nsize.z) / 20 };
    const byte3 jaw_size = { (3 * nsize.x) / 5, (2 * nsize.y) / 5, (18 * nsize.z) / 25 };
    // the neck: a fill centred on the bottom edge, its lower half cut off, widest at the bottom (into the shoulders)
    //  (a box's corner and size round apart when scaled to the model, it came out lopsided)
    const byte3 neck_centre = { nsize.x / 2, 0, (11 * nsize.z) / 25 };
    const byte3 neck_size = { (9 * nsize.x) / 25, (16 * nsize.y) / 25, (9 * nsize.z) / 25 };
    entity e2 = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, cranium_centre, cranium_size, 1);
    zox_set(e2, NodeDepth, { depth });
    new_link_single_node(world, e, e2);
    entity jaw = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, jaw_centre, jaw_size, 1);
    zox_set(jaw, NodeDepth, { depth });
    new_link_single_node(world, e2, jaw);
    entity neck = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, neck_centre, neck_size, 1);
    zox_set(neck, NodeDepth, { depth });
    new_link_single_node(world, jaw, neck);
    e2 = neck;
    // spawn eyes
    byte eye_place_type = zox_model_node_paint; // fill | paint;
    byte eye_ridge = nsize.x / 8;
    byte eye_size = nsize.x / 7;
    // mid face, the neck's under it
    byte eye_pos_y = (29 * nsize.y) / 50;
    if (eye_ridge == 0) {
        eye_ridge = 1;
    }
    if (eye_size == 0) {
        eye_size = 1;
    }
    // painted from the head's middle to its front: paint only colours solid voxels, so the eyes land on the face
    //  wherever its surface is (a jaw or cranium shape moves it), the inside painted unseen
    byte eye_pos_z = nsize.z / 2;
    const byte eye_depth = nsize.z - eye_pos_z;
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
    entity e3 = spawn_node_model_colors(world, eye_color, 1);
    new_link_single_node(world, e2, e3);
    const byte3 eye_box = { eye_size, eye_size, eye_depth };
    entity e4 = spawn_node_model_at(world, prefab_node_model, eye_place_type, leye_position, eye_box, 2);
    new_link_single_node(world, e3, e4);
    entity e5 = spawn_node_model_at(world, prefab_node_model, eye_place_type, reye_position, eye_box, 2);
    new_link_single_node(world, e4, e5);
    // zox_log("Eye size[%i] L[%i] R[%i] out of [%i]... Ridge [%i]", eye_size, leye_position.x, reye_position.x, nsize.x, eye_ridge);
    // zox_log("=> eye - ridge [%i] size [%i] at [(L:%ixR:%i)x%ix%i] nsize[%ix%ix%i]", eye_ridge, eye_size, leye_position.x, reye_position.x, eye_pos_y, eye_pos_z, nsize.x, nsize.y, nsize.z);
    // Finish Him!
    entity end_node = spawn_node_model_end(world);
    new_link_single_node(world, e5, end_node);
    return e;
}
