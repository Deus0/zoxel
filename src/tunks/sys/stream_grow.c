// Should I Chunk2Neighbors, Chunk2Position
// Tunk2 a child of Chunk2 entities
zox_sys2(TunksSpawnSystem) {
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TunkPosition);
    zox_sys_in(RenderDistance);
    zox_sys_out(Chunk2Neighbors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDistance, rdistance);
        zox_sys_i(TunkPosition, cposition);
        zox_sys_o(Chunk2Neighbors, neighbors);
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
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
        zox_geter(terrain, TunkLinks, chunks);
        for (byte j = 0; j < 4; j++) {
            entity neighbor = neighbors->value[j];
            // no need to spawn if neighbor exists
            if (zox_valid(neighbor)) {
                continue;
            }
            // get position of neighbor and check terrain for it
            int2 neighbor_position = int2_add(cposition->value, get_direction_int2(j));
            neighbor = int2_hashmap_get(chunks->value, neighbor_position);
            // if not existing yet, spawn a new chunk
            if (zox_valid(neighbor)) {
                neighbors->value[j] = neighbor;
                continue;
            }
            byte had_streamer = 0;
            int2 closest_point;
            float closest_distance = 9999;
            zox_sys_query_begin();
            while (zox_sys_query_loop()) {
                zox_sys_begin_2();
                zox_sys_in_2(StreamerLevel);
                zox_sys_in_2(StreamLink);
                zox_sys_in_2(StreamPosition2);
                for (int k = 0; k < it2.count; k++) {
                    zox_sys_i_2i(StreamerLevel, level, k);
                    zox_sys_i_2i(StreamLink, stream_terrain, k);
                    zox_sys_i_2i(StreamPosition2, stream_point, k);
                    if (level->value < 0) {
                        continue;
                    }
                    if (stream_terrain->value != terrain) {
                        continue;
                    }
                    float distance = int2_distance(stream_point->value, neighbor_position);
                    if (distance < closest_distance) {
                        had_streamer = 1;
                        closest_distance = distance;
                        closest_point = stream_point->value;
                    }
                }
            }
            zox_sys_query_end();
            if (!had_streamer) {
                continue;
            }
            // int2 stream_point = find_closest_point2(streamers, streamers_count, nposition);
            // only spawn new chunk if within stream distance
            byte streamer_distance = get_camera_chunk2_distance(closest_point, neighbor_position);
            // zox_log("Streaming - nposition [%ix%i] rdistance [%i] streamer [%ix%i]", nposition.x, nposition.y, rdistance, stream_point.x, stream_point.y);
            if (streamer_distance <= terrain_lod_far) {
                byte lod = camera_distance_to_terrain_render_depth(streamer_distance);
                entity tunk = spawn_tunk(world, prefab_tunk2, terrain, 0, neighbor_position, streamer_distance, lod);
                int2_hashmap_add(chunks->value, neighbor_position, tunk);
                neighbors->value[j] = neighbor;
                // zox_log("+  New Tunk at [%ix%i]", nposition.x, nposition.y);
            }
        }
    }
} zox_sys_end(TunksSpawnSystem);
