// NOTE: Generates lights again when the neighbor lights update
zox_sys2(ChunkNeighborLightTriggerSystem) {
    if (disable_lights) {
        return;
    }
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LightNodeDirty);
    zox_sys_in(ChunkNeighbors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LightNodeDirty, dirty);
        zox_sys_i(ChunkNeighbors, neighbors);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        for (byte j = 0; j < chunk_neighbors_length; j++) {
            entity neighbor = neighbors->value[j];
            if (!zox_valid(neighbor) || !zox_has(neighbor, MeshColorsGenerate)) {
                continue;
            }
            iter it2 = zox_children(world, neighbor);
            while (zox_children_next(it2)) {
                for (int k = 0; k < it2.count; k++) {
                    entity e3 = it2.entities[k];
                    if (!zox_has(e3, ChunkMesh)) {
                        continue;
                    }
                    zox_set(e3, MeshColorsGenerate, { 1 });
                    if (dbg_log) {
                        zox_log("Neighbor Chunk Triggered Build [%s] > [%s]:[%s]", zox_getn(e), zox_getn(neighbor), zox_getn(e3));
                    }
                }
            }
        }
    }
} zox_sys_end(ChunkNeighborLightTriggerSystem);
