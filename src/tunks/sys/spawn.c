// Should I Chunk2Neighbors, Chunk2Position
// Tunk2 a child of Chunk2 entities
zox_sys2(Tunk2SpawnSystem) {
    zox_sys_query();
    zox_sys_world();
    int2 *streamers = NULL;
    int streamers_count = 0;
    byte iterated = 0;
    //  TODO: We should add these not just use first array
    zox_sys_query_begin();
    while (zox_sys_query_loop()) {
        if (iterated) {
            continue;
        }
        iterated = 1;
        zox_sys_begin_2();
        zox_sys_in_2(StreamPoint2);
        streamers = (int2*) StreamPoint2s_2;
        streamers_count = it2.count;
    }
    zox_sys_query_end();
    if (!streamers_count) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(VoxLink);
    zox_sys_in(Chunk2Position);
    zox_sys_in(RenderDistance);
    zox_sys_out(Chunk2Neighbors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxLink, terrain);
        zox_sys_i(RenderDistance, rdistance);
        zox_sys_i(Chunk2Position, cposition);
        zox_sys_o(Chunk2Neighbors, neighbors);
        if (!zox_valid(terrain->value)) {
            continue;
        }
        byte is_invalids = 0;
        for (byte j = 0; j < 4; j++) {
            entity neighbor = neighbors->value[j];
            if (!zox_valid(neighbor)) {
                is_invalids = 1;
                break;
            }
        }
        if (!is_invalids) {
            continue;
        }
        // This is what determines when to grow or not
        byte stream_zone = rdistance->value < terrain_lod_far;
        if (!stream_zone) {
            continue;
        }
        zox_geter(terrain->value, TunkLinks, chunks);
        for (byte j = 0; j < 4; j++) {
            entity neighbor = neighbors->value[j];
            // no need to spawn if neighbor exists
            if (zox_valid(neighbor)) {
                continue;
            }
            // get position of neighbor and check terrain for it
            int2 nposition = int2_add(cposition->value, get_direction_int2(j));
            neighbor = int2_hashmap_get(chunks->value, nposition);
            // if not existing yet, spawn a new chunk
            if (!zox_valid(neighbor)) {
                int2 stream_point = find_closest_point2(streamers, streamers_count, nposition);
                // only spawn new chunk if within stream distance
                byte rdistance = get_camera_chunk2_distance(stream_point, nposition);
                // zox_log("Streaming - nposition [%ix%i] rdistance [%i] streamer [%ix%i]", nposition.x, nposition.y, rdistance, stream_point.x, stream_point.y);
                if (rdistance <= terrain_lod_far) {
                    entity chunk = spawn_tunk(world, prefab_tunk2, terrain->value, nposition, rdistance);
                    //  zox_geter(terrain->value, TunkLinks, chunks);
                    int2_hashmap_add(chunks->value, nposition, chunk);
                    // zox_log("+  New Tunk at [%ix%i]", nposition.x, nposition.y);
                }
            }
            neighbors->value[j] = neighbor;
        }
    }
} zox_sys_end(Tunk2SpawnSystem);
