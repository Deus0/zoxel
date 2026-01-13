// Should I Chunk2Neighbors, Chunk2Position

// Tunk2 a child of Chunk2 entities

zox_sys2(Tunk2SpawnSystem) {
    // const entity prefab_chunk = prefab_chunk_terrain;

    zox_sys_query();
    zox_sys_world();

    const byte log_individuals = 0;
    uint spawned_chunks = 0;
    int3 *stream_points = NULL;
    int stream_points_length = 0;
    byte iterated = 0;

    zox_sys_query_begin();
    while (zox_sys_query_loop()) {
        if (!iterated) {
            iterated = 1;
            zox_sys_begin_2();
            zox_sys_in_2(StreamPoint);
            stream_points = (int3*) StreamPoints_2;
            stream_points_length = it2.count;
        }
    }

    zox_sys_query_end();
    if (stream_points_length == 0) {
        return;
    }

    zox_sys_begin();
    zox_sys_in(VoxLink);
    zox_sys_in(RenderDistance);
    zox_sys_in(Chunk2Position);
    zox_sys_out(Chunk2Neighbors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxLink, terrain);
        zox_sys_i(RenderDistance, rdistance);
        zox_sys_i(Chunk2Position, cposition);
        zox_sys_o(Chunk2Neighbors, neighbors);

        if (!zox_valid(terrain->value)) {
            continue;
        }

        // Pass if loading chunk
        if (rdistance->value == 255) {
            continue;
        }

        // zox_geter_value(terrain->value, BlockScale, float, terrain_scale);

        // This is what determines when to grow or not
        const byte stream_zone = rdistance->value < terrain_lod_far;

        if (stream_zone) {
            for (byte j = 0; j < 4; j++) {
                entity neighbor = neighbors->value[j];

                // no need to spawn if neighbor exists
                if (zox_valid(neighbor)) {
                    continue;
                }

                // get position of neighbor and check terrain for it
                const int2 nposition = int3_add(cposition->value, get_direction_int2(j));

                zox_geter(terrain->value, ChunkLinks2, chunks);
                neighbor = int3_hashmap_get(chunks->value, nposition);

                // if not existing yet, spawn a new chunk
                if (!zox_valid(neighbor)) {

                    const int2 stream_point = find_closest_point2(
                        stream_points,
                        stream_points_length,
                        nposition);

                    // only spawn new chunk if within stream distance
                    const byte camera_distance = get_camera_chunk_distance_xz(
                        stream_point,
                        nposition);

                    if (camera_distance <= terrain_lod_far) {

                        entity chunk = 0; // spawn_tunk2(world, prefab_tunk2, nposition);
                        /*spawn_chunk_terrain(
                            world,
                            prefab_chunk,
                            terrain->value,
                            stream_point,
                            nposition,
                            terrain_depth,
                            terrain_scale
                        );*/

                        zox_geter(terrain->value, ChunkLinks2, chunks);
                        int2_hashmap_add(chunks->value, nposition, chunk);

                        spawned_chunks++;
                    }
                }
                neighbors->value[j] = neighbor;
            }
        }
    }

    if (spawned_chunks > 0) {
        zox_log_streaming(" + [%i] spawned [%i]", ecs_run_count, spawned_chunks);
    }

} zox_sys_end(Tunk2SpawnSystem);