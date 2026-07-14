entity spawn_model(ecs *world, entity prefab, byte mdepth, byte ndepth, byte3 rsize) {
    byte ddepth = mdepth - ndepth;
    float bscale = ((float) powers_of_two[ddepth]) / 64.0f;
    // zox_log("Model at depth [%i/%i] has ddepth [%i] and scale [%f]", ndepth, mdepth, ddepth, bscale);
    zox_instance(prefab);
    zox_set(e, NodeDepth, { ndepth });
    zox_set(e, BlockScale, { bscale });
    zox_set(e, ChunkSize, { byte3_to_int3(rsize) });
    // NOTE: Do instanced models need these??
    spawn_gpu_mesh(world, e);
    spawn_gpu_colors(world, e);
    return e;
}

// TODO: Remove Model Lods and just use Models with children Render Meshes
entity spawn_model_lods(ecs* world, color c, lint seed, byte mdepth, byte3 rsize, const char* label, ModelLods* mlods) {
    srand(seed);
    c = color_mutate(c, 40);
    zox_make_new();
    zox_name("model_lods");
    zox_set(e, Seed, { seed });
    zox_set(e, MaxRenderDepth, { mdepth });
    for (byte i = 0; i <= mdepth; i++) {
        byte rdepth = i;
        byte ddepth = powers_of_two[mdepth - rdepth];
        byte3 rsized = rsize;
        rsized.x /= ddepth;
        rsized.y /= ddepth;
        rsized.z /= ddepth;
        entity e2 = spawn_model(world, prefab_vox, mdepth, rdepth, rsized);
        zox_set_parent(world, e2, e);
        zox_set_unique_name(e2, label);
        // zox_set(e2, GenerateModel, { zox_generate_model_run });
        zox_set(e2, RenderDepth, { rdepth });  // move this to prefab
        zox_set(e2, Seed, { seed });
        zox_set(e2, Color, { c });
        zox_set(e2, MaxRenderDepth, { mdepth });
        mlods->value[i] = e2;
    }
    zox_set_ptr(e, ModelLods, *mlods);
    return e;
}
