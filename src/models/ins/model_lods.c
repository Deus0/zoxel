entity spawn_model(ecs *world, entity prefab, byte mdepth, byte ndepth, byte3 rsize) {
    byte ddepth = mdepth - ndepth;
    short length = octree_size(ddepth);
    float bscale = ((float) length) / 64.0f;
    zox_instance(prefab);
    zox_set(e, NodeDepth, { ndepth });
    zox_set(e, BlockScale, { bscale });
    zox_set(e, ChunkSize, { byte3_to_int3(rsize) });
    return e;
}

// TODO: Remove Model Lods and just use Models with children Render Meshes
entity spawn_model_lods(ecs* world, entity parent, entity prefab_lod, color c, lint seed, byte mdepth, byte3 rsize, const char* label, ModelLods* mlods) {
    // entity prefab_lod = prefab_invisible_vox;
    srand(seed);
    c = color_mutate(c, 40);
    zox_make_new();
    zox_name("model_lods");
    zox_set_parent(world, e, parent);
    zox_set(e, Seed, { seed });
    zox_set(e, MaxRenderDepth, { mdepth });
    for (byte i = 0; i <= mdepth; i++) {
        byte rdepth = i;
        short ddepth = octree_size(mdepth - rdepth);
        byte3 rsized = rsize;
        rsized.x /= ddepth;
        rsized.y /= ddepth;
        rsized.z /= ddepth;
        entity e2 = spawn_model(world, prefab_lod, mdepth, rdepth, rsized);
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