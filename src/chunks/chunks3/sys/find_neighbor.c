// NOTE: New chunks will find neighbors, and set themselves on  their neighbors
void chunk_neighbors_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkPosition);
    zox_sys_out(ChunkNeighbors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkPosition, position);
        zox_sys_o(ChunkNeighbors, neighbors);
        entity terrain = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_loge("Terrain invalid in finding neighbors");
            continue;
        }
#endif
        if (!zox_has(terrain, ChunkLinks)) {
            zox_loge("Vox [%s] has no ChunkLinks in FindNeighbors",
                zox_getn(terrain));
            continue;
        }
        // link up neighbors if they need to be
        zox_muter(terrain, ChunkLinks, chunks);
        // Just reversing directions
        const byte neighbor_indexes[] = { 1, 0, 3, 2, 5, 4 };
        for (byte j = 0; j < chunk_neighbors_length; j++) {
            if (zox_valid(neighbors->value[j])) {
                continue;
            }
            int3 neighbor_position = int3_add(position->value, get_direction_int3(j));
            entity neighbor = int3_hashmap_get(chunks->value, neighbor_position);
            // Edge of map
            if (!zox_valid(neighbor)) {
                if (dbg_log) {
                    zox_log("[%s]'s Neighbor [%i] Not Found",
                        zox_sys_e_name,
                        j);
                }
                continue;
            }
            neighbors->value[j] = neighbor;
            // we can add ourself to the neighbor here
            byte neighbor_index = neighbor_indexes[j];zox_muter(neighbor, ChunkNeighbors, neighbor_neighbors);
            entity old_neighbors_neighbor =
                neighbor_neighbors->value[neighbor_index];
            if (!zox_valid(old_neighbors_neighbor)) {
                neighbor_neighbors->value[neighbor_index] = e;
            }
            if (dbg_log) {
                zox_log("[%s]'s Neighbor [%i] Set [%s]",
                    zox_sys_e_name,
                    j,
                    zox_getn(neighbor));
            }
        }
        zox_remove(e, FindNeighbors);
    }
    zox_sys_on_end();
} zoxd_system(chunk_neighbors_system);
