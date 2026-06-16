// doesn't check for edge chunks
zox_sys2(ChunkFindNeighborSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkPosition);
    zox_sys_in(RenderDepth);
    zox_sys_out(ChunkNeighbors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkPosition, chunkPosition);
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_o(ChunkNeighbors, neighbors);
        entity terrain = zox_get_parent(world, e);
        // todo: use 255 and 254, 254 for invisible and 255 for initiated
        // rdepth->value == render_depth_invisible ||
        if (!terrain) {
            continue;
        }
        byte need_find = 0;
        for (byte j = 0; j < chunk_neighbors_length; j++) {
            if (!zox_valid(neighbors->value[j])) {
                need_find = 1;
                break;
            }
        }
        if (!need_find) {
            continue;
        }
        // link up neighbors if they need to be
        zox_muter(terrain, ChunkLinks, chunks);
        for (byte j = 0; j < chunk_neighbors_length; j++) {
            if (zox_valid(neighbors->value[j])) {
                continue;
            }
            int3 nposition = int3_add(chunkPosition->value, get_direction_int3(j));
            entity neighbor = int3_hashmap_get(chunks->value, nposition);
            if (zox_valid(neighbor)) {
                neighbors->value[j] = neighbor;
            }
        }
    }
} zox_sys_end(ChunkFindNeighborSystem);
