entity2 spawn_model_grass(ecs* world, lint seed, byte max_depth, color vox_color) {
    entity vox_lod = 0;
    byte vlength = powers_of_two[max_depth];
    int voxels = 10 * vlength * vlength + rand() % vlength * vlength;
    zox_make_new();
    zox_set_unique_name(e, "model_grass");
    zox_set(e, MaxRenderDepth, { max_depth });
    ModelLods modelLods;
    for (byte i = 0; i <= max_depth; i++) {
        byte rdepth = i;
        byte node_length = powers_of_two[rdepth];
        byte ddepth = max_depth - rdepth;
        int place_count = voxels / (pow(ddepth + 1, 2 + ddepth));
        if (place_count == 0) {
            place_count = 1;
        }
        entity e2 = spawn_vox_basic(world, prefab_vox, rdepth, max_depth);
        zox_set_unique_name(e2, "model_lod_grass");
        zox_set(e2, Seed, { seed });
        zox_set(e2, VoxType, { vox_type_rubble });
        zox_set(e2, Generate, { zox_dirty_trigger });
        zox_set(e2, Color, { vox_color });
        zox_set(e2, RubbleCount, { place_count });
        zox_set(e2, RubbleHeight, { node_length - 1 });
        zox_set(e2, RenderDepth, { rdepth });
        zox_set(e2, MaxRenderDepth, { max_depth });
        modelLods.value[i] = e2;
        if (i == max_depth) {
            vox_lod = e2;
        }
    }
    zox_set_ptr(e, ModelLods, modelLods);
    return (entity2) { e, vox_lod };
}
