// Vox data children of an owner entity, linked with VoxLink
//  - the owner keeps transform and RenderDepth, its children render or hold voxels

// The lod mesh an owner renders at a depth, its own vox if it has one, otherwise its model
entity get_owner_lod_mesh(
    ecs* world,
    entity e,
    byte depth)
{
    if (depth >= model_lods_max_length) {
        return 0;
    }
    entity source = zox_has(e, VoxLink) ? zox_getv(e, VoxLink) : 0;
    if (!zox_valid(source) || !zox_has(source, ModelLods)) {
        source = zox_has(e, ModelLink) ? zox_getv(e, ModelLink) : 0;
    }
    if (!zox_valid(source) || !zox_has(source, ModelLods)) {
        return 0;
    }
    zox_geter(source, ModelLods, lods);
    return lods->value[depth];
}

// Spawns a child vox to combine voxes into
entity spawn_vox_combined_child(
    ecs* world,
    entity parent)
{
    entity e = zox_ins(world, prefab_vox_combined);
    zox_set_unique_name(e, "vox_combined");
    initialize_voxel_lock(world, e);
    // NOTE: Mesh building reads this from the vox
    if (zox_has(parent, NoSplitChunk)) {
        zox_add(e, NoSplitChunk);
    }
    zox_set_parent(world, e, parent);
    zox_setv(parent, VoxLink, e);
    return e;
}

// Spawns a child copy of a model with its own lod meshes, so it can be changed
//  - lods are returned as VoxLink is deferred
entity spawn_vox_cloned_child(
    ecs* world,
    entity parent,
    entity model,
    ModelLods* lods)
{
    if (!zox_valid(model) || !zox_has(model, VoxelNode)) {
        zox_loge("Cannot clone invalid model for [%s]", zox_getn(parent));
        return 0;
    }
    byte max_depth = zox_has(model, MaxRenderDepth) ?
        zox_getv(model, MaxRenderDepth) :
        zox_getv(model, NodeDepth);
    entity e = spawn_vox_lods(world, parent, "clone", max_depth, lods);
    zox_setv(e, CloneVoxLink, model);
    zox_setv(e, CloneVox, 1);
    zox_setv(parent, VoxLink, e);
    return e;
}
