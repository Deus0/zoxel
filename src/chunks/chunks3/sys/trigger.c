// NOTE: Sets Mesh Children to build when Voxels are dirty
void chunk_sides_trigger_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_add(e, BuildChunkSides);
        if (dbg_log) {
            zox_log("Chunk Triggered Build Sides + Mesh [%s]", zox_getn(e));
        }
    }
    zox_sys_on_end();
} zoxd_system(chunk_sides_trigger_system);

// should i grab neighbor states instead, or should i create setters?
//  probably grab them, its faster to write in my systems
void chunk_neighbors_sides_trigger_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkNeighbors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkNeighbors, neighbors);
        for (byte j = 0; j < chunk_neighbors_length; j++) {
            entity neighbor = neighbors->value[j];
            if (!zox_valid(neighbor) || !zox_has(neighbor, Chunk3)) {
                if (dbg_log) {
                    zox_loge("Chunk [%s] has No Neighbor [%i]", zox_get_name(e), j);
                }
                continue;
            }
            zox_add(neighbor, BuildChunkSides);
            if (dbg_log) {
                zox_log("Chunk [%s] Triggered Neighbors Build Sides + Mesh [%s]", zox_getn(e), zox_getn(neighbor));
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(chunk_neighbors_sides_trigger_system);
