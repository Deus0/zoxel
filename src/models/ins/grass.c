entity2 spawn_model_grass(
    ecs* world,
    entity prefab_lod,
    lint seed,
    byte max_depth,
    color vox_color)
{
    byte dbg_log = 0;
    entity vox_lod = 0;
    byte length = octree_size(max_depth);
    srand(seed);
    uint place_count_big = rand_range((length * 0.2f) * length * length, length * length * (length * 0.6f));
    entity e = zox_new();
    zox_set_unique_name(e, "model_grass");
    zox_add(e, Model);
    zox_setv(e, MaxRenderDepth, max_depth);
    ModelLods lods = { 0 };
    for (byte depth = 0; depth <= max_depth; depth++) {
        short node_length = octree_size(depth);
        byte ddepth = max_depth - depth;
        uint place_count = place_count_big / (pow(ddepth + 1, 2 + ddepth));
        if (place_count == 0) {
            place_count = 1;
        }
        entity e2 = spawn_vox(
            world,
            prefab_lod,
            depth,
            max_depth);
        zox_set_unique_name(e2, "model_lod_grass");
        zox_setv(e2, Seed, seed);
        zox_setv(e2, VoxType, vox_type_rubble);
        zox_setv(e2, GenerateModel, zox_generate_model_run);
        zox_setv(e2, Color, vox_color);
        zox_setv(e2, RubbleHeight, node_length - 1);
        zox_setv(e2, RenderDepth, depth);
        zox_setv(e2, MaxRenderDepth, max_depth);
        zox_setv(e2, RubbleCount, place_count);
        lods.value[depth] = e2;
        if (depth == max_depth) {
            vox_lod = e2;
        }
        if (dbg_log) {
            zox_log("Model Grass - Depth [%i]", depth);
        }
    }
    zox_set_ptr(e, ModelLods, lods);
    return (entity2) {
        e,
        vox_lod
    };
}
