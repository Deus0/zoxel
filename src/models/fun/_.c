static inline bool in_bounds(byte3 p, byte size) {
    return p.x < size && p.y < size && p.z < size;
}

// TODO: Move to octree macros
static inline void set_voxel_safe(VoxelNode *tree, byte depth, byte3 p, byte v) {
    if (!in_bounds(p, octree_size(depth))) {
        // zox_logw("Position [%ix%ix%i] out of B [%i]", p.x, p.y, p.z, depth);
        return;
    }
    set_VoxelNode(tree, depth, p, v);
}

// graphs
#include "nodegraph.c"
// nodes
#include "paint.c"
#include "cube.c"
#include "sphere.c"
#include "ellipsoid.c"
#include "cylinder.c"

entity spawn_blueprint_models(
    ecs *world,
    entity parent,
    entity nodegraph,
    lint seed,
    const char *name,
    byte depth,
    byte3 size,
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
        entity model = spawn_model_lods(
            world,
            model_group,
            prefab_vox,
            color_red,
            variant_seed,
            depth,
            size,
            name,
            &lods);
        zox_set_unique_name(model, name);
        add_to_ModelLinks(&variants, model);
        spawn_process_model(world, prefab_process_model, nodegraph, model);
    }
    zox_set_ptr(model_group, ModelLinks, variants);
    return model_group;
}