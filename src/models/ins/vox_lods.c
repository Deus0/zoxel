// Spawns a single max detail vox, with a lod mesh per render depth
//  - voxels are only generated / set at max_depth
//  - its meshes are its children (get_model_lod_mesh finds one by depth, for instancing); out_lods returns them now,
//    as children spawned in a system aren't found till the merge
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

// gives a vox_lods model its material ids (MaterialIDs, a material a palette entry, as MagicaVoxel's) and a see
//  through mesh beside each lod (TransparentMesh, the same RenderDepth) for its see through voxels; its solid meshes
//  rebuild without them. Only for models with more than solid (others need no ids, no second meshes)
void set_vox_lods_materials(ecs* world, entity vox, const byte* ids, int count) {
    MaterialIDs material_ids = { 0 };
    resize_MaterialIDs(&material_ids, count);
    memcpy(material_ids.value, ids, count);
    zox_set_ptr(vox, MaterialIDs, material_ids);    // copied in
    dispose_MaterialIDs(&material_ids);
    const byte max_depth = zox_getv(vox, NodeDepth);
    entity meshes[model_lods_max_length] = { 0 };
    byte found[model_lods_max_length] = { 0 };
    iter it = zox_children(world, vox);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity child = it.entities[i];
            if (!zox_has(child, VoxLodMesh)) {
                continue;
            }
            const byte depth = zox_getv(child, RenderDepth);
            if (depth < model_lods_max_length) {
                found[depth] |= zox_has(child, TransparentMesh);
                if (!zox_has(child, TransparentMesh)) {
                    meshes[depth] = child;
                }
            }
        }
    }
    for (byte depth = 0; depth <= max_depth && depth < model_lods_max_length; depth++) {
        if (meshes[depth]) {
            zox_add(meshes[depth], BuildMesh);
        }
        if (found[depth]) {
            continue;
        }
        const entity mesh = zox_ins(world, prefab_vox_mesh);
        zox_set_unique_name(mesh, "vox_mesh_see_through");
        zox_add(mesh, TransparentMesh);
        zox_add(mesh, BuildMesh);
        zox_setv(mesh, RenderDepth, depth);
        zox_set_parent(world, mesh, vox);
    }
}
