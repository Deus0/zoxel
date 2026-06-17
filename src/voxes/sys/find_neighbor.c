// doesn't check for edge chunks
zox_sys2(ChunkFindNeighborSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeEntity);
    zox_sys_in(ChunkPosition);
    zox_sys_out(ChunkNeighbors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(InitializeEntity, state);
        zox_sys_i(ChunkPosition, position);
        zox_sys_o(ChunkNeighbors, neighbors);
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        if (!terrain) {
            continue;
        }
        /*byte need_find = 0;
        for (byte j = 0; j < chunk_neighbors_length; j++) {
            if (!zox_valid(neighbors->value[j])) {
                need_find = 1;
                break;
            }
        }
        if (!need_find) {
            continue;
        }*/
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
            if (zox_valid(neighbor)) {
                neighbors->value[j] = neighbor;
                // If not initializing Neighbor
                if (!zox_getv(neighbor, InitializeEntity)) {
                    // we can add ourself to the neighbor here
                    byte neighbor_index = neighbor_indexes[j];
                    zox_muter(neighbor, ChunkNeighbors, neighbor_neighbors);
                    neighbor_neighbors->value[neighbor_index] = e;
                }
            }
        }
    }
} zox_sys_end(ChunkFindNeighborSystem);
