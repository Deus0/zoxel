// should i grab neighbor states instead, or should i create setters?
//  probably grab them, its faster to write in my systems
// TODO: Optimize this uses alot per frame atm
/*zox_sys2(Chunk3NeighborsMeshTriggerSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(ChunkMeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(ChunkMeshDirty, mesh_dirty);
        for (byte j = 0; j < chunk_neighbors_length; j++) {
            entity neighbor = neighbors->value[j];
            if (!zox_valid(neighbor) || !zox_has(neighbor, VoxelNodeDirty)) {
                continue;
            }
            byte neighbor_dirty = zox_getv(neighbor, VoxelNodeDirty);
            if (neighbor_dirty == zox_dirty_active) {
                mesh_dirty->value = zox_dirty_trigger;
                break;
            }
        }
    }
} zox_sys_end(Chunk3NeighborsMeshTriggerSystem);*/

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
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        for (byte j = 0; j < chunk_neighbors_length; j++) {
            entity neighbor = neighbors->value[j];
            if (!zox_valid(neighbor) || !zox_has(neighbor, ChunkMeshDirty)) {
                if (dbg_log) {
                    zox_loge("Chunk [%s] has No Neighbor [%i]", zox_get_name(e), j);
                }
                continue;
            }
            zox_muter(neighbor, ChunkMeshDirty, mesh_dirty);
            mesh_dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(Chunk3NeighborsMeshTriggerSystem);
