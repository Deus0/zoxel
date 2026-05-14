entity spawn_model_grass(ecs* world, color c, lint seed) {
    srand(seed);
    c = color_mutate(c, 40);
    byte mdepth = block_vox_depth_limits.y;
    byte spawn_depth = 5;
    byte vlength = powers_of_two[spawn_depth]; // max_render_depth];
    int voxels = 10 * vlength * vlength + rand() % vlength * vlength;
    zox_make_new();
    zox_name("model_grass");
    zox_set(e, MaxRenderDepth, { mdepth });
    ModelLods modelLods;
    for (byte i = 0; i <= mdepth; i++) {
        byte rdepth = i;
        byte node_length = powers_of_two[rdepth];
        byte ddepth = spawn_depth - rdepth;
        int place_count = voxels / (pow(ddepth + 1, 2 + ddepth));
        if (place_count == 0) {
            place_count = 1;
        }
        entity e2 = spawn_vox_basic(world, prefab_vox, rdepth, mdepth);
        zox_set_unique_name(e2, "model_lod_grass");
        zox_set(e2, RenderDepth, { rdepth });
        zox_set(e2, MaxRenderDepth, { mdepth });
        zox_set(e2, Seed, { seed });
        zox_set(e2, VoxType, { vox_type_rubble });
        zox_set(e2, Generate, { zox_dirty_trigger });
        zox_set(e2, Color, { c });
        zox_set(e2, RubbleCount, { place_count });
        zox_set(e2, RubbleHeight, { node_length - 1 });
        modelLods.value[i] = e2;
    }
    zox_set_ptr(e, ModelLods, modelLods);
    return e;
}
