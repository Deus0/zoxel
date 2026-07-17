// should i grab neighbor states instead, or should i create setters?
//  probably grab them, its faster to write in my systems
// When: VoxelNodeDirty is zox_dirty_active
zox_sys2(Chunk3MeshTriggerSystem) {
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    zox_sys_out(BuildChunkMesh);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxelNodeDirty, vdirty);
        zox_sys_o(BuildChunkMesh, build_mesh);
        // if node dirty, or for now, if node depth dirty... wait we dont want to double up
        if (vdirty->value == zox_dirty_active) {
            build_mesh->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(Chunk3MeshTriggerSystem);

// NOTE: Sets Mesh Children to build when Voxels are dirty
zox_sys2(Chunk3MeshTrigger2System) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    zox_sys_out(BuildChunkSides);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNodeDirty, dirty);
        zox_sys_o(BuildChunkSides, build);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        build->value = 1;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (zox_has(e2, ChunkMesh)) {
                    zox_set(e2, BuildChunkMesh, { zox_dirty_trigger });
                }
            }
        }
    }
} zox_sys_end(Chunk3MeshTrigger2System);
