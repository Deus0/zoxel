// NOTE: Get a list of block ids in octree, then get a list of materials from that
// NOTE: Finally spawn mesh if not already in children for that depth
// NOTE: For now just spawn a child mesh per lod if it doesnt exist

// Call for each dirty flag
static inline void spawn_chunk_meshes(
    ecs* world,
    entity e,
    const VoxelNode* voxels,
    byte depth,
    float4x4 matrix,
    byte render_disabled,
    byte dbg_log)
{
    const byte is_cull_air = 1;
    // check any solids
    // TODO: Only spawn mesh if sides exist!
    if (is_cull_air && !voxels->value) {
        if (dbg_log) {
            zox_log(" - Chunk is Air for [%s] Depth [%i]", zox_getn(e), depth);
        }
        return;
    }
    entity lod_mesh = 0;
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity mesh = it2.entities[j];
            if (!zox_has(mesh, ChunkMesh)) {
                continue;
            }
            if (zox_getv(mesh, RenderDepth) == depth) {
                lod_mesh = mesh;
                break;
            }
        }
        if (lod_mesh) {
            break;
        }
    }
    if (lod_mesh) {
        if (dbg_log) {
            zox_log(" - Chunk Mesh Existed for [%s] at depth [%i]", zox_getn(e), depth);
        }
        return;
    }
    entity terrain = zox_get_parent(world, e);
#ifdef zox_safety_checks
    if (!zox_valid(terrain)) {
        zox_loge("terrain Invalid for ChunkMesh %s", zox_getn(e));
        return;
    }
    if (!zox_has(terrain, TilemapLink)) {
        zox_loge("e [%s] terrain has no TilemapLink %s", zox_getn(e), zox_getn(terrain));
        return;
    }
#endif
    entity material = zox_getv(terrain, TilemapLink);
    if (!zox_valid(material)) {
        zox_loge("Tilemap Material is Invalid");
        return;
    }
    // NOTE: When Depth changes or Voxels Generate, if mesh doesnt exist we spawn new
    entity e2 = zox_ins(world, prefab_chunk_mesh_textured);
    zox_set_unique_name(e2, "chunk_mesh");
    zox_set_parent(world, e2, e);
    zox_setv(e2, RenderDepth, depth);
    zox_setv(e2, RenderDisabled, render_disabled);
    zox_setv(e2, TransformMatrix, matrix);
    zox_setv(e2, MaterialLink, material);
    if (dbg_log) {
        zox_log(" - New Chunk Mesh for [%s] at depth [%i]", zox_getn(e), depth);
    }
}

// TODO: Keep a list of materials per chunk
// TODO: Use sides instead of voxels for check if need mesh
// If voxels changed, we need to spawn mesh for it

zox_sys2(ChunkMeshSpawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(RenderDisabled);
    zox_sys_in(RenderDepth);
    zox_sys_in(VoxelNode);
    zox_sys_in(VoxelNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(RenderDisabled, render_disabled);
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(VoxelNode, voxels);
        zox_sys_i(VoxelNodeDirty, dirty);
        if (dirty->value != zox_dirty_end) {
            continue;
        }
        spawn_chunk_meshes(
            world,
            e,
            voxels,
            depth->value,
            matrix->value,
            render_disabled->value,
            dbg_log);
        if (dbg_log) {
            zox_log("Spawned Chunk Mesh on [%s] Depth [%i] Disabled [%i]", zox_getn(e), depth->value, render_disabled->value);
        }
    }
} zox_sys_end(ChunkMeshSpawnSystem);


zox_sys2(ChunkMeshSpawn2System) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(RenderDisabled);
    zox_sys_in(RenderDepth);
    zox_sys_in(VoxelNode);
    zox_sys_out(ChunkLodDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(RenderDisabled, render_disabled);
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(VoxelNode, voxels);
        zox_sys_o(ChunkLodDirty, dirty);
        if (dirty->value != zox_chunk_lod_dirty_spawn) {
            continue;
        }
        spawn_chunk_meshes(
            world,
            e,
            voxels,
            depth->value,
            matrix->value,
            render_disabled->value,
            dbg_log);
        dirty->value = zox_chunk_lod_dirty_toggle;
        if (dbg_log) {
            zox_log("Spawned Chunk Mesh 2 on [%s] Depth [%i]", zox_getn(e), depth->value);
        }
    }
} zox_sys_end(ChunkMeshSpawn2System);

