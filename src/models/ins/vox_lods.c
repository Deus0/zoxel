// Spawns a single max detail vox, with a lod mesh per render depth
//  - voxels are only generated / set at max_depth
//  - ModelLods links each depth to its mesh, used for instancing
entity spawn_vox_lods(
    ecs *world,
    entity parent,
    const char* name,
    byte max_depth,
    ModelLods* out_lods)
{
    if (max_depth >= model_lods_max_length) {
        zox_logw("Vox [%s] depth [%i] past max lods [%i]",
            name,
            max_depth,
            model_lods_max_length);
        max_depth = model_lods_max_length - 1;
    }
    ushort length = octree_size(max_depth);
    zox_instance(prefab_vox_lods);
    {
        char name2[64];
        snprintf(name2, sizeof(name2), "model_%s", name);
        zox_name(name2);
    }
    zox_setv(e, NodeDepth, max_depth);
    zox_setv(e, RenderDepth, max_depth);
    zox_setv(e, MaxRenderDepth, max_depth);
    zox_setv(e, ChunkSize, int3_single(length));
    zox_setv(e, BlockScale, vox_model_scale);
    initialize_voxel_lock(world, e);
    if (parent) {
        zox_set_parent(world, e, parent);
    }
    ModelLods lods = { 0 };
    for (byte depth = 0; depth <= max_depth; depth++) {
        entity mesh = zox_ins(world, prefab_vox_mesh);
        zox_set_unique_name(mesh, "vox_mesh");
        zox_setv(mesh, RenderDepth, depth);
        zox_set_parent(world, mesh, e);
        lods.value[depth] = mesh;
    }
    zox_set_ptr(e, ModelLods, lods);
    if (out_lods) {
        *out_lods = lods;
    }
    return e;
}

// Makes all lod meshes sway, for grass
// NOTE: Pass in lods from spawn_vox_lods, as sets are deferred in systems
void set_vox_lods_sway(
    ecs *world,
    const ModelLods* lods,
    float2 sway_bounds)
{
    for (byte depth = 0; depth < model_lods_max_length; depth++) {
        entity mesh = lods->value[depth];
        if (!mesh) {
            break;
        }
        zox_add(mesh, SwayMesh);
        zox_setv(mesh, SwayBounds, sway_bounds);
    }
}
