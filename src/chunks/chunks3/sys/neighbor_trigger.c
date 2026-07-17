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
            //zox_muter(neighbor, BuildChunkMesh, mesh_dirty);
            //mesh_dirty->value = zox_dirty_trigger;
            zox_setm(neighbor, BuildChunkSides, 1);
            iter it2 = zox_children(world, neighbor);
            while (zox_children_next(it2)) {
                for (int k = 0; k < it2.count; k++) {
                    entity e3 = it2.entities[k];
                    if (zox_has(e3, ChunkMesh)) {
                        zox_set(e3, BuildChunkMesh, { zox_dirty_trigger });
                    }
                }
            }
        }
    }
} zox_sys_end(Chunk3NeighborsMeshTriggerSystem);
