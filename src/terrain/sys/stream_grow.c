// extern entity spawn_chunk_terrain(ecs*, entity, entity, int3, int3, byte, float);
// NOTE: Spawns chunks from other chunks with ChunkNeighbors!
/*zox_sys2(ChunkSpawnSystem) {
    byte dbg_log = 0;
    uint spawned_chunks = 0;
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDistanceDirty);
    zox_sys_in(RenderDistance);
    zox_sys_in(ChunkPosition);
    zox_sys_out(ChunkNeighbors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDistanceDirty, dirty);
        zox_sys_i(RenderDistance, render_distance);
        zox_sys_i(ChunkPosition, chunk_position);
        zox_sys_o(ChunkNeighbors, neighbors);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        // Pass if loading chunk
        if (render_distance->value == 255) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            if (dbg_log >= 2) {
                zox_log("Invalid Terrain on Chunk", zox_get_name(e));
            }
            continue;
        }
        float terrain_scale = zox_getv(terrain, BlockScale);
        byte stream_zone = render_distance->value < terrain_lod_far;
        if (!stream_zone) {
            if (dbg_log >= 2) {
                zox_log("Out of Stream Zone: Camera Distance: [%i] Render Distance [%i]", render_distance->value, terrain_lod_far);
            }
            continue;
        }
        for (byte j = 0; j < 6; j++) {
            entity neighbor = neighbors->value[j];
            // no need to spawn if neighbor exists
            if (zox_valid(neighbor)) {
                continue;
            }
            // get position of neighbor and check terrain for it
            int3 direction = get_direction_int3(j);
            int3 neighbor_position = int3_add(chunk_position->value, direction);
            if (!(neighbor_position.y >= -render_distance_y && neighbor_position.y <= render_distance_y)) {
                if (!(chunk_position->value.y >= -render_distance_y && chunk_position->value.y <= render_distance_y)) {
                    zox_loge("Spawned Chunk out of Range of Y", neighbor_position.y);
                }
                continue;
            }
            zox_geter(terrain, ChunkLinks, chunks);
            neighbor = int3_hashmap_get(chunks->value, neighbor_position);
            // if not existing yet, spawn a new chunk
            if (zox_valid(neighbor)) {
                neighbors->value[j] = neighbor;
                continue;
            }
            // NOTE: Uses Query Directly
            byte had_streamer = 0;
            int3 closest_point;
            float closest_distance = 9999;
            zox_sys_query_begin();
            while (zox_sys_query_loop()) {
                zox_sys_begin_2();
                zox_sys_in_2(StreamerLevel);
                zox_sys_in_2(StreamLink);
                zox_sys_in_2(StreamPosition);
                for (int k = 0; k < it2.count; k++) {
                    zox_sys_i_2i(StreamerLevel, level, k);
                    zox_sys_i_2i(StreamLink, stream_terrain, k);
                    zox_sys_i_2i(StreamPosition, stream_point, k);
                    // had_any_streamers = 1;
                    if (level->value < 1) {
                        continue;
                    }
                    if (stream_terrain->value != terrain) {
                        continue;
                    }
                    float distance = int3_distance(stream_point->value, neighbor_position);
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
            // only spawn new chunk if within stream distance
            byte camera_distance = get_camera_chunk_distance_xz(closest_point, neighbor_position);
            if (camera_distance <= terrain_lod_far) {
                byte render_depth = camera_distance_to_terrain_render_depth(camera_distance);
                neighbor = spawn_terrain_chunk(world, prefab_chunk_terrain, terrain, neighbor_position, terrain_depth, terrain_scale, camera_distance, render_depth);
                int3_hashmap_add(chunks->value, neighbor_position, neighbor);
                neighbors->value[j] = neighbor;
                if (dbg_log >= 2) {
                    zox_log("Streaming: new [%i]s chunk [%ix%ix%i]", spawned_chunks, neighbor_position.x, neighbor_position.y, neighbor_position.z);
                }
                spawned_chunks++;
            } else {
                if (dbg_log >= 2) {
                    zox_log("Distance out of range, Camera Distance: [%i] Render Distance [%i]", camera_distance, terrain_lod_far);
                }
            }
        }
    }
    if (dbg_log && spawned_chunks > 0) {
        zox_log("ChunkSpawnSystem [%i] Spawned [%i]", ecs_run_count, spawned_chunks);
    }
} zox_sys_end(ChunkSpawnSystem);*/
