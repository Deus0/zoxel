entity spawn_model_grass(ecs* world, color c, lint seed) {
    // properties
    srand(seed);
    c = color_mutate(c, 40);
    byte max_render_depth = block_vox_depth;
    byte vlength = powers_of_two[max_render_depth];

    int big_rubbles = 10 * vlength * vlength + rand() % vlength * vlength;

    // spawn model
    zox_make_new();
    zox_name("model_grass");
    zox_set(e, MaxRenderDepth, { max_render_depth });

    // spawn model lod variants
    ModelLods modelLods;
    for (int i = 0; i <= max_render_depth; i++) {
        byte render_depth = i; // max_render_depth - i;
        byte node_length = powers_of_two[render_depth];
        int place_count = big_rubbles / (pow((max_render_depth - i) + 1, 2 + (max_render_depth - i)));
        if (place_count == 0) {
            place_count = 1;
        }

        entity e2 = spawn_vox_basic(world, prefab_vox, max_render_depth, render_depth);

        zox_set_unique_name(e2, "model_lod_grass");

        // default
        zox_set(e2, RenderDepth, { render_depth });  // move this to prefab
        zox_set(e2, MaxRenderDepth, { render_depth });
        zox_set(e2, Seed, { seed });
        zox_set(e2, VoxType, { vox_type_rubble });
        zox_set(e2, Generate, { zox_dirty_trigger });
        // important
        zox_set(e2, Color, { c });
        zox_set(e2, RubbleCount, { place_count });
        zox_set(e2, RubbleHeight, { node_length - 1 });
        modelLods.value[i] = e2;
    }

    zox_set_ptr(e, ModelLods, modelLods);

    return e;
}
