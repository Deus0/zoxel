entity spawn_model_generated(ecs* world, entity parent, byte vox_type, color fill, byte depth, byte max_depth, lint seed) {
    entity e = spawn_vox_basic(world, prefab_vox, depth, max_depth);
    zox_set_unique_name(e, "model_rubble");
    zox_set_parent(world, e, parent);
    zox_set(e, VoxType, { vox_type });
    zox_set(e, Color, { fill });
    zox_set(e, RenderDepth, { depth });
    zox_set(e, MaxRenderDepth, { max_depth });
    zox_set(e, Generate, { zox_dirty_trigger });
    zox_set(e, Seed, { seed });
    return e;
}

entity2 spawn_model_lods_generated(ecs* world, entity parent, byte vox_type, color fill, byte max_depth, lint seed) {
    entity e = zox_new();
    zox_set_unique_name(e, "model_lods");
    zox_set(e, MaxRenderDepth, { max_depth });
    entity max_depth_vox = 0;
    ModelLods lods;
    for (byte depth = 0; depth <= max_depth; depth++) {
        entity e2 = spawn_model_generated(world, e, vox_type, fill, depth, max_depth, seed);
        lods.value[depth] = e2;
        if (depth == max_depth) {
            max_depth_vox = e2;
        }
    }
    zox_set_ptr(e, ModelLods, lods);
    return (entity2) { e, max_depth_vox };
}
