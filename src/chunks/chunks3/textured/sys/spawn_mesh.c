// NOTE: Get a list of block ids in octree, then get a list of materials from that
// NOTE: Finally spawn mesh if not already in children for that depth
// NOTE: For now just spawn a child mesh per lod if it doesnt exist
zox_sys2(ChunkMeshSpawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_in(RenderDepthDirty);
    // zox_sys_in(VoxelNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(RenderDepthDirty, render_depth_dirty);
        // zox_sys_i(VoxelNodeDirty, voxels_dirty);
        if (render_depth_dirty->value != zox_dirty_active) {
            continue;
        }
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
                if (zox_getv(mesh, RenderDepth) != depth->value) {
                    // zox_set(e2, RenderDisabled, { 1 });
                    zox_disable(mesh);
                } else {
                    zox_enable(mesh);
                    // zox_set(mesh, RenderDisabled, { 0 });
                    lod_mesh = mesh;
                }
            }
        }
        if (lod_mesh) {
            if (dbg_log) {
                zox_log("Chunk Mesh Existed for [%s] at depth [%i]", zox_getn(e), depth->value);
            }
            continue;
        }
        // Will build mesh right away
        entity e2 = zox_ins(world, prefab_chunk_mesh_textured);
        zox_set_unique_name(e2, "chunk_mesh");
        zox_set_parent(world, e2, e);
        zox_set(e2, RenderDepth, { depth->value });
        spawn_gpu_mesh(world, e2);
        spawn_gpu_uvs(world, e2);
        spawn_gpu_colors(world, e2);
        if (dbg_log) {
            zox_log("Spawned new Chunk Mesh for [%s] at depth [%i]", zox_getn(e), depth->value);
        }
    }
} zox_sys_end(ChunkMeshSpawnSystem);
