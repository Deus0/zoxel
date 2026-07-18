// NOTE: Get a list of block ids in octree, then get a list of materials from that
// NOTE: Finally spawn mesh if not already in children for that depth
// NOTE: For now just spawn a child mesh per lod if it doesnt exist
zox_sys2(ChunkMeshSpawnSystem) {
    if (zox_dbg_disable_chunk_mesh) {
        return;
    }
    if (zox_combine_chunk_mode) {
        return;
    }
    // TODO: Only spawn mesh if sides exist!
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(RenderDisabled);
    // zox_sys_in(RenderDepthDirty);
    zox_sys_in(VoxelNodeDirty);
    zox_sys_in(RenderDepth);
    zox_sys_in(VoxelNode);
    zox_sys_out(ChunkLodDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(RenderDisabled, render_disabled);
        // zox_sys_i(RenderDepthDirty, render_depth_dirty);
        zox_sys_i(VoxelNodeDirty, voxels_dirty);
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(VoxelNode, voxels);
        zox_sys_o(ChunkLodDirty, dirty);
        if (dirty->value != 1) {
            continue;
        }
        dirty->value = 2;
        /*if (!(render_depth_dirty->value == zox_dirty_active || voxels_dirty->value == zox_dirty_end)) {
            continue;
        }
        // check any solids
        if (!voxels->value) {
            continue;
        }*/
        entity lod_mesh = 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            if (lod_mesh) {
                continue;
            }
            for (int j = 0; j < it2.count; j++) {
                entity mesh = it2.entities[j];
                if (!zox_has(mesh, ChunkMesh)) {
                    continue;
                }
                if (zox_getv(mesh, RenderDepth) == depth->value) {
                    lod_mesh = mesh;
                    break;
                }
            }
        }
        if (lod_mesh) {
            if (dbg_log) {
                zox_log("Chunk Mesh Existed for [%s] at depth [%i]", zox_getn(e), depth->value);
            }
            continue;
        }
        // NOTE: When Depth changes or Voxels Generate, if mesh doesnt exist we spawn new
        entity e2 = zox_ins(world, prefab_chunk_mesh_textured);
        zox_set_unique_name(e2, "chunk_mesh");
        zox_set_parent(world, e2, e);
        zox_set(e2, TransformMatrix, { matrix->value });
        zox_set(e2, RenderDepth, { depth->value });
        zox_set(e2, RenderDisabled, { render_disabled->value });
        spawn_gpu_mesh(world, e2);
        spawn_gpu_uvs(world, e2);
        spawn_gpu_colors(world, e2);
        if (dbg_log) {
            zox_log("Spawned new Chunk Mesh for [%s] at depth [%i]", zox_getn(e), depth->value);
        }
    }
} zox_sys_end(ChunkMeshSpawnSystem);

// NOTE: Toggles the meshes beased on render depth
zox_sys2(ChunkMeshToggleSystem) {
    if (zox_combine_chunk_mode) {
        return;
    }
    // TODO: Only spawn mesh if sides exist!
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_out(ChunkLodDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_o(ChunkLodDirty, dirty);
        if (dirty->value != 2) {
            continue;
        }
        dirty->value = 0;
        continue;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity mesh = it2.entities[j];
                if (!zox_has(mesh, ChunkMesh)) {
                    continue;
                }
                if (zox_getv(mesh, RenderDepth) != depth->value) {
                    zox_disable(mesh);
                } else {
                    zox_enable(mesh);
                }
            }
        }
        dirty->value = 0;
    }
} zox_sys_end(ChunkMeshToggleSystem);

