

entity spawn_model2(ecs* world, entity parent, lint seed, byte max_depth, byte depth) {
    byte ddepth = max_depth - depth;
    short length = octree_size(ddepth);
    float bscale = ((float) length) / 64.0f;
    zox_instance(prefab_vox);
    zox_set_parent(world, e, parent);
    zox_set(e, Seed, { seed });
    zox_set(e, NodeDepth, { depth });
    zox_set(e, RenderDepth, { depth });
    zox_set(e, BlockScale, { bscale });
    zox_set(e, MaxRenderDepth, { max_depth });
    // zox_set(e, ChunkSize, { byte3_to_int3(rsize) });
    return e;
}

// color c,
entity spawn_model_lods2(
    ecs* world,
    entity parent,
    lint seed,
    byte max_depth,
    const char* label,
    ModelLods* mlods)
{
    // srand(seed);
    // c = color_mutate(c, 40);
    entity e = zox_new();
    zox_name("model_lods");
    zox_set_parent(world, e, parent);
    zox_set(e, Seed, { seed });
    zox_set(e, MaxRenderDepth, { max_depth });
    for (ushort depth = 0; depth <= max_depth; depth++) {
        // byte depth = i;
        entity e2 = spawn_model2(world, e, seed, max_depth, depth);
        zox_set_unique_name(e2, label);
        // zox_set(e2, Color, { c });
        mlods->value[depth] = e2;
    }
    zox_set_ptr(e, ModelLods, *mlods);
    return e;
}

entity spawn_blueprint_models(
    ecs *world,
    entity parent,
    entity nodegraph,
    lint seed,
    const char *name,
    byte depth,
    // byte3 size,
    byte variants_count)
{
    lint variant_seed_step = 1209;
    entity model_group = zox_ins(world, prefab_model_group);
    zox_set_unique_name(model_group, name);
    zox_set_parent(world, model_group, parent);
    ModelLinks variants = { 0 };
    for (byte i = 0; i < variants_count; i++) {
        lint variant_seed = seed + (lint) i * variant_seed_step;
        ModelLods lods = { 0 };
        entity model = spawn_model_lods2(
            world,
            model_group,
            // color_red,
            variant_seed,
            depth,
            // size,
            name,
            &lods);
        zox_set_unique_name(model, name);
        add_to_ModelLinks(&variants, model);
        // NOTE: Kicks off model generaiton using the blueprint
        spawn_process_model(
            world,
            nodegraph,
            model);
    }
    zox_set_ptr(model_group, ModelLinks, variants);
    return model_group;
}
