// When Mesh Rebuilds, rebuild our colors too based on lights
// If queue empty + lightnode dirty, activate!
zox_sys2(ChunkColorsTriggerSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LightNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LightNodeDirty, lights_dirty);
        if (lights_dirty->value != zox_dirty_active) {
            continue;
        }
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (zox_has(e2, ChunkMesh)) {
                    continue;
                }
                zox_add(e2, MeshColorsGenerate);
                if (dbg_log) {
                    zox_log("Chunk Triggered Build [%s] > [%s]:[%s]", zox_getn(e), zox_getn(e2));
                }
            }
        }
    }
} zox_sys_end(ChunkColorsTriggerSystem);

// NOTE: Generates lights again when the neighbor lights update
zox_sys2(ChunkNeighborLightTriggerSystem) {
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
            if (!zox_valid(neighbor)) {
                continue;
            }
            iter it2 = zox_children(world, neighbor);
            while (zox_children_next(it2)) {
                for (int k = 0; k < it2.count; k++) {
                    entity mesh = it2.entities[k];
                    if (!zox_has(mesh, ChunkMesh)) {
                        continue;
                    }
                    zox_add(mesh, MeshColorsGenerate);
                    if (dbg_log) {
                        zox_log("Neighbor Chunk Triggered Build [%s] > [%s]:[%s]", zox_getn(e), zox_getn(neighbor), zox_getn(mesh));
                    }
                }
            }
        }
    }
} zox_sys_end(ChunkNeighborLightTriggerSystem);

