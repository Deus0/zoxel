// Grass is generated once at max depth, lods are meshed from that
entity spawn_model_grass(
    ecs* world,
    lint seed,
    byte max_depth,
    color vox_color)
{
    byte length = octree_size(max_depth);
    zox_srand(seed);
    // stalks: 12 to 22 percent of its columns (it filled most columns, a solid mass of them)
    uint place_count = rand_range(length * length * 0.12f, length * length * 0.22f);
    if (place_count == 0) {
        place_count = 1;
    }
    ModelLods lods = { 0 };
    entity e = spawn_vox_lods(
        world,
        0,
        "grass",
        max_depth,
        &lods);
    zox_setv(e, Seed, seed);
    zox_setv(e, VoxType, vox_type_rubble);
    zox_setv(e, GenerateModel, zox_generate_model_run);
    zox_setv(e, Color, vox_color);
    zox_setv(e, RubbleHeight, length - 1);
    zox_setv(e, RubbleCount, place_count);
    // sway animation - mesh is centered, so bottom is -bounds.y
    // NOTE: same size/scale as spawn_vox_lods, its sets are deferred so we can't read them back here
    float3 bounds = calculate_vox_bounds(
        int3_single(length),
        vox_model_scale);
    set_vox_lods_sway(
        world,
        &lods,
        (float2) { -bounds.y, bounds.y * 2 });
    return e;
}
