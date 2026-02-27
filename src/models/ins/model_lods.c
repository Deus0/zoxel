entity spawn_model(ecs *world, entity p, byte mdepth, byte ndepth, byte3 rsize) {

    byte ddepth = mdepth - ndepth + 1;
    float bscale = ((float) ddepth) / 64.0f;

    zox_instance(p);
    zox_set(e, NodeDepth, { ndepth });
    zox_set(e, BlockScale, { bscale });
    zox_set(e, ChunkSize, { byte3_to_int3(rsize) });

    // NOTE: Do instanced models need these??
    spawn_gpu_mesh(world, e);
    spawn_gpu_colors(world, e);

    return e;
}

entity spawn_model_lods(ecs* world, color c, lint seed, byte mdepth, byte3 rsize, const char* label, ModelLods* mlods) {
    // properties
    srand(seed);
    c = color_mutate(c, 40);

    // byte mdepth = block_vox_depth;

    // spawn model
    zox_make_new();
    zox_name("model_lods");
    zox_set(e, Seed, { seed });
    zox_set(e, MaxRenderDepth, { mdepth });

    // spawn model lod variants
    // ModelLods lods;

    for (int i = 0; i <= mdepth; i++) {
        byte rdepth = i;
        byte node_length = powers_of_two[rdepth];
        byte ddepth = mdepth - rdepth + 1;

        byte3 rsized = rsize;

        rsized.x /= ddepth;
        rsized.y /= ddepth;
        rsized.z /= ddepth;

        entity e2 = spawn_model(world, prefab_vox, mdepth, rdepth, rsized);
        zox_set_unique_name(e2, label); // "model_lod");

        // default
        zox_set(e2, RenderDepth, { rdepth });  // move this to prefab
        zox_set(e2, Seed, { seed });
        zox_set(e2, Color, { c });

        // Not sure why this needed a max render depth
        //  - EXPLAIN: Basically in character spawning, uses camera_distance_to_npc_render_depth with camera to set, so needs to be max here!
        zox_set(e2, MaxRenderDepth, { mdepth });

        mlods->value[i] = e2;

        /*rsize.x /= 2;
        rsize.y /= 2;
        rsize.z /= 2;

        if (rsize.x == 0) rsize.x = 1;
        if (rsize.y == 0) rsize.y = 1;
        if (rsize.z == 0) rsize.z = 1;*/
    }

    zox_set_ptr(e, ModelLods, *mlods);

    return e;
}
