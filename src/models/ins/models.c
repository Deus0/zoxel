entity spawn_model2(
    ecs* world,
    entity parent,
    lint seed,
    byte max_depth,
    byte depth)
{
    byte ddepth = max_depth - depth;
    short length = octree_size(ddepth);
    float bscale = ((float) length) / 64.0f;

    entity e = spawn_vox(
        world,
        prefab_vox,
        depth,
        max_depth);
    /*zox_instance(prefab_vox);
     z ox_set(e, NodeDepth, { depth });         *
     zox_set(e, RenderDepth, { depth });
     zox_set(e, BlockScale, { bscale });
     zox_set(e, MaxRenderDepth, { max_depth });*/
    zox_set_parent(world, e, parent);
    zox_set(e, Seed, { seed });
    // zox_set(e, ChunkSize, { byte3_to_int3(rsize) });
    return e;
}

// color c,
entity2 spawn_model_lods2(
    ecs* world,
    entity parent,
    lint seed,
    byte max_depth,
    const char* label)
{
    // srand(seed);
    // c = color_mutate(c, 40);
    entity e = zox_new();
    zox_name("model_lods");
    zox_add(e, Model);
    zox_setv(e, Seed, seed);
    zox_setv(e, MaxRenderDepth, max_depth);
    zox_set_parent(world, e, parent);
    entity vox = 0;
    ModelLods lods = { 0 };
    for (ushort depth = 0; depth <= max_depth; depth++) {
        entity e2 = spawn_model2(
            world,
            e,
            seed,
            max_depth,
            depth);
        zox_set_unique_name(e2, label);
        lods.value[depth] = e2;
        if (depth == max_depth) {
            vox = e2;
        }
    }
    zox_set_ptr(e, ModelLods, lods);
    return (entity2) {
        e,
        vox
    };
}

entity3 spawn_blueprint_models(
    ecs *world,
    entity parent,
    entity nodegraph,
    lint seed,
    const char *name,
    byte depth,
    byte variants_count)
{
    lint variant_seed_step = 1209;
    entity2 first_models;
    entity model_group = zox_ins(world, prefab_model_group);
    zox_set_unique_name(model_group, name);
    zox_set_parent(world, model_group, parent);
    ModelLinks variants = { 0 };
    for (byte i = 0; i < variants_count; i++) {
        lint variant_seed = seed + (lint) i * variant_seed_step;
        entity2 models = spawn_model_lods2(
            world,
            model_group,
            variant_seed,
            depth,
            name);
        // zox_set_unique_name(model, name);
        // NOTE: Kicks off model generaiton using the blueprint
        spawn_process_model(
            world,
            nodegraph,
            models.x);
        add_to_ModelLinks(&variants, models.x);
        if (i == 0) {
            first_models = models;
        }
    }
    zox_set_ptr(model_group, ModelLinks, variants);
    return (entity3) {
        model_group,
        first_models.x,
        first_models.y
    };
}
