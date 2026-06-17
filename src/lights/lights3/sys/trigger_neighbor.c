// NOTE: Generates lights again when the neighbor lights update
/*zox_sys2(ChunkNeighborLightTriggerSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(MeshColorsGenerate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(MeshColorsGenerate, generate);
        for (byte j = 0; j < chunk_neighbors_length; j++) {
            entity neighbor = neighbors->value[j];
            if (!zox_valid(neighbor) || !zox_has(neighbor, LightNodeDirty)) {
                continue;
            }
            byte neighbor_dirty = zox_getv(neighbor, LightNodeDirty);
            if (neighbor_dirty == zox_dirty_active) {
                generate->value = zox_dirty_trigger;
                break;
            }
        }
    }
} zox_sys_end(ChunkNeighborLightTriggerSystem);*/

zox_sys2(ChunkNeighborLightTriggerSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LightNodeDirty);
    zox_sys_in(ChunkNeighbors);
    for (int i = 0; i < it->count; i++) {
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
            zox_muter(neighbor, MeshColorsGenerate, mesh_dirty);
            mesh_dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(ChunkNeighborLightTriggerSystem);
