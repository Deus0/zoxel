// NOTE: Sets Mesh Children to build when Voxels are dirty
zox_sys2(ChunkMeshTriggerSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    // zox_sys_out(BuildChunkSides);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNodeDirty, dirty);
        // zox_sys_o(BuildChunkSides, build);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        // build->value = 1;
        zox_add(e, BuildChunkSides);
        if (dbg_log) {
            zox_log("Chunk Triggered Build Sides + Mesh [%s]", zox_getn(e));
        }
    }
} zox_sys_end(ChunkMeshTriggerSystem);

// should i grab neighbor states instead, or should i create setters?
//  probably grab them, its faster to write in my systems
zox_sys2(Chunk3NeighborsMeshTriggerSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    zox_sys_in(ChunkNeighbors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNodeDirty, dirty);
        zox_sys_i(ChunkNeighbors, neighbors);
        // if (dirty->value != zox_dirty_end) {
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        for (byte j = 0; j < chunk_neighbors_length; j++) {
            entity neighbor = neighbors->value[j];
            if (!zox_valid(neighbor) || !zox_has(neighbor, Chunk3)) {
                if (dbg_log) {
                    zox_loge("Chunk [%s] has No Neighbor [%i]", zox_get_name(e), j);
                }
                continue;
            }
            zox_add(neighbor, BuildChunkSides);
        }
    }
} zox_sys_end(Chunk3NeighborsMeshTriggerSystem);
