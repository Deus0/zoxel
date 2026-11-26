// Spawns a model group, of variants generated with the nodegraph blueprint
//  - x: model group
//  - y: first model variant
// models' materials (set_vox_lods_materials): a material id a palette entry, the see through one's material
typedef struct {
    const byte* ids;
    int count;
    entity see_through;
} model_materials;

// a model group of variants made from a node graph; materials (NULL: all solid) for ones with see through voxels
entity2 spawn_blueprint_models_materials(
    ecs *world,
    entity parent,
    entity nodegraph,
    lint seed,
    const char *name,
    byte depth,
    byte variants_count,
    const model_materials* materials)
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
        if (materials) {
            set_new_vox_lods_materials(world, model, depth, materials->ids, materials->count, materials->see_through);
        }
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

entity2 spawn_blueprint_models(ecs *world, entity parent, entity nodegraph, lint seed, const char *name, byte depth,
    byte variants_count)
{
    return spawn_blueprint_models_materials(world, parent, nodegraph, seed, name, depth, variants_count, NULL);
}

// Spawns an animation: a VoxAnimation (a model group) of frames, each the nodegraph run on one, all from one seed
//  - its children are its frames, ModelFrame 0 to frames - 1, played at fps (vodes3 animate)
//  - active: its frames ModelActive, shown while their block is on (a rune lit, eve/rune.c), its ActiveModelLink
//  - x: the animation, y: its first frame
entity2 spawn_blueprint_animation(ecs *world, entity parent, entity nodegraph, lint seed, const char *name, byte depth,
    byte frames, byte fps, byte active)
{
    entity first_frame = 0;
    entity animation = zox_ins(world, prefab_vox_animation);
    zox_set_unique_name(animation, name);
    zox_setv(animation, AnimationFps, fps);
    zox_set_parent(world, animation, parent);
    for (byte i = 0; i < frames; i++) {
        entity frame = spawn_vox_lods(world, animation, name, depth, NULL);
        zox_setv(frame, Seed, seed);
        zox_setv(frame, ModelFrame, i);
        if (active) {
            zox_add(frame, ModelActive);
        }
        spawn_process_model(world, nodegraph, frame);
        if (!i) {
            first_frame = frame;
        }
    }
    return (entity2) { animation, first_frame };
}
