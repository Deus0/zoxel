entity spawn_model_lods(ecs* world, color c, lint seed) {
    // properties
    srand(seed);
    c = color_mutate(c, 40);
    byte mdepth = block_vox_depth;

    // spawn model
    zox_make_new();
    zox_name("model_lods");
    zox_set(e, Seed, { seed });
    zox_set(e, MaxRenderDepth, { mdepth });

    // spawn model lod variants
    ModelLods lods;

    for (int i = 0; i <= mdepth; i++) {
        byte rdepth = i;
        byte node_length = powers_of_two[rdepth];

        entity e2 = spawn_vox_basic(
            world,
            prefab_vox,
            mdepth,
            rdepth
        );

        zox_set_unique_name(e2, "model_lod");

        // default
        zox_set(e2, RenderDepth, { rdepth });  // move this to prefab
        zox_set(e2, Seed, { seed });
        zox_set(e2, Color, { c });

        // Not sure why this needed a max render depth
        //  - EXPLAIN: Basically in character spawning, uses camera_distance_to_npc_render_depth with camera to set, so needs to be max here!
        zox_set(e2, MaxRenderDepth, { mdepth });

        lods.value[i] = e2;
    }

    zox_set_ptr(e, ModelLods, lods);

    return e;
}