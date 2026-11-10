// Spawns a model group, of variants generated with the nodegraph blueprint
//  - x: model group
//  - y: first model variant
entity2 spawn_blueprint_models(
    ecs *world,
    entity parent,
    entity nodegraph,
    lint seed,
    const char *name,
    byte depth,
    byte variants_count)
{
    lint variant_seed_step = 1209;
    entity first_model = 0;
    entity model_group = zox_ins(world, prefab_model_group);
    zox_set_unique_name(model_group, name);
    zox_set_parent(world, model_group, parent);
    for (byte i = 0; i < variants_count; i++) {
        lint variant_seed = seed + (lint) i * variant_seed_step;
        entity model = spawn_vox_lods(
            world,
            model_group,
            name,
            depth,
            NULL);
        zox_setv(model, Seed, variant_seed);
        // NOTE: Kicks off model generaiton using the blueprint
        spawn_process_model(
            world,
            nodegraph,
            model);
        if (i == 0) {
            first_model = model;
        }
    }
    return (entity2) {
        model_group,
        first_model
    };
}
