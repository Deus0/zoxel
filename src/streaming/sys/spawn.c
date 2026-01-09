extern entity spawn_chunk_terrain(ecs*, const entity, const entity, const int3, const int3, const byte, const float);

// NOTE: Takes in array of stream points and iterates per chunk
// NOTE: This logic fails if all chunks dissapear

zox_sys2(ChunkSpawnSystem) {
    const entity prefab_chunk = prefab_chunk_terrain;

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
    zox_sys_in(ChunkPosition);
    zox_sys_in(VoxLink);
    zox_sys_in(RenderDistance);
    zox_sys_out(ChunkNeighbors);

    for (int i = 0; i < it->count; i++) {

        // zox_sys_e();
        zox_sys_i(VoxLink, voxLink);
        zox_sys_i(RenderDistance, rdistance);
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_o(ChunkNeighbors, neighbors);

        if (!zox_valid(voxLink->value)) {
            continue;
        }

        // Pass if loading chunk
        if (rdistance->value == 255) {
            continue;
        }

        // Pass if lod changing
        /*zox_geter_value(e, ChunkLodDirty, byte, chunkLodDirty);
        if (chunkLodDirty != 0) {
            continue;
        }*/

        zox_geter_value(voxLink->value, BlockScale, float, terrain_scale);
        const byte stream_zone = rdistance->value < terrain_lod_far;
        if (stream_zone) {
            for (byte j = 0; j < 6; j++) {
                entity neighbor = neighbors->value[j];

                // no need to spawn if neighbor exists
                if (zox_valid(neighbor)) {
                    continue;
                }

                // get position of neighbor and check terrain for it
                const int3 neighbor_position = int3_add(cposition->value, get_direction_int3(j));
                if (!(neighbor_position.y >= -render_distance_y && neighbor_position.y <= render_distance_y)) {
                    continue;
                }

                zox_geter(voxLink->value, ChunkLinks, oldChunkLinks);
                neighbor = int3_hashmap_get(oldChunkLinks->value, neighbor_position);

                // if not existing yet, spawn a new chunk
                if (!zox_valid(neighbor)) {

                    const int3 stream_point = find_closest_point(
                        stream_points,
                        stream_points_length,
                        neighbor_position);

                    // only spawn new chunk if within stream distance
                    const byte camera_distance = get_camera_chunk_distance_xz(
                        stream_point,
                        neighbor_position);

                    if (camera_distance <= terrain_lod_far) {

                        neighbor = spawn_chunk_terrain(
                            world,
                            prefab_chunk,
                            voxLink->value,
                            stream_point,
                            neighbor_position,
                            terrain_depth,
                            terrain_scale
                        );

                        zox_geter(voxLink->value, ChunkLinks, chunkLinks);
                        int3_hashmap_add(chunkLinks->value, neighbor_position, neighbor);
                        if (log_individuals) {
                            zox_log_streaming("+ streaming: new [%i]s chunk [%ix%ix%i]", spawned_chunks, neighbor_position.x, neighbor_position.y, neighbor_position.z);
                        }
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

} zox_sys_end(ChunkSpawnSystem);
