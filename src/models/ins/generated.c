entity spawn_model_generated(ecs* world, entity parent, const char* name, lint seed, byte vox_type, color fill, byte depth, byte max_depth) {
    entity e = spawn_vox_basic(world, prefab_invisible_vox, depth, max_depth);
    {
        char name2[64];
        sprintf(name2, "model_%s", name);
        zox_name(name2);
    }
    zox_set_parent(world, e, parent);
    zox_set(e, Seed, { seed });
    zox_set(e, VoxType, { vox_type });
    zox_set(e, Color, { fill });
    zox_set(e, GenerateModel, { zox_generate_model_run });
    zox_set(e, RenderDepth, { depth });
    zox_set(e, MaxRenderDepth, { max_depth });
    return e;
}

entity2 spawn_model_lods_generated(ecs* world, entity parent, const char* name, byte vox_type, color fill, byte max_depth, lint seed) {
    entity e = zox_new();
    {
        char name2[64];
        sprintf(name2, "modellods_%s", name);
        zox_name(name2);
    }
    zox_set_unique_name(e, "model_lods");
    zox_set(e, MaxRenderDepth, { max_depth });
    entity texture_model = 0;
    ModelLods lods;
    for (byte depth = 0; depth <= max_depth; depth++) {
        entity e2 = spawn_model_generated(world, e, name, seed, vox_type, fill, depth, max_depth);
        lods.value[depth] = e2;
        if (depth == max_depth) {
            texture_model = e2;
        }
    }
    zox_set_ptr(e, ModelLods, lods);
    return (entity2) { e, texture_model };
}
