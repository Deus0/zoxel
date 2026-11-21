// TODO Mark neighbors of spawning chunks as Dirty (NeighborsDirty)

#define terrain_streamers_max 16

// the chunk columns of a terrain's streamers that load land (StreamerLevel 1+)
static inline byte terrain_stream_points(ecs* world, query* query, entity terrain, int2* points) {
    byte count = 0;
    zox_sys_query_begin();
    while (zox_sys_query_loop()) {
        zox_sys_begin_2();
        zox_sys_in_2(StreamDirty);    // the filter's fields in order (streamers, terrain/sys/_.c)
        (void) StreamDirtys_2;
        zox_sys_in_2(StreamerLevel);
        zox_sys_in_2(StreamPosition);
        for (int j = 0; j < it2.count; j++) {
            zox_sys_i_2(StreamerLevel, level);
            zox_sys_i_2(StreamPosition, point);
            if (level->value >= 1 && count < terrain_streamers_max &&
                zox_get_link(world, it2.entities[j], StreamLink) == terrain) {
                points[count++] = (int2) { point->value.x, point->value.z };
            }
        }
    }
    zox_sys_query_end();
    return count;
}

// a column's distance to its closest streamer (local coop: one streamer set the columns round it to its own distance
//  while another was closer, so their lods fought each time either moved, tst coop_streamers)
static inline int terrain_stream_distance(int2 position, int distance, const int2* points, byte count) {
    for (byte k = 0; k < count; k++) {
        const int other = int2_distance(points[k], position);
        if (other < distance) {
            distance = other;
        }
    }
    return distance;
}

// the row offset (z) of a sweep's row r: the centre first, then out each side (0, -1, 1, -2, 2 ..)
static inline int terrain_stream_row_offset(int r) {
    return r == 0 ? 0 : (r & 1 ? -(r + 1) / 2 : r / 2);
}

#define zox_terrain_stream_budget_ms 1.0   // queueing a sweep's rows, a frame (a terrain)

// NOTE: Queues every column round a streamer that moved (StreamDirty) to spawn or update, spread over frames: rows of
//  columns, the centre ones first, within a budget a frame (StreamRow: the next row, -1 none; StreamSwept: from where)
//  - a sweep at far 32 is 65 x 65 columns (~6.6ms at once); it ran whole on all 3 frames of StreamDirty (trigger,
//    active, end), now once a move, a little a frame
void terrain_stream_system(iter* it) {
    byte dbg_log = 0;
    const int size = terrain_lod_far;
    const int rows = size * 2 + 1;
    zox_sys_on_begin();
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TunkLinks);
    zox_sys_out(TerrainSpawnQueue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TunkLinks, tunks);
        zox_sys_o(TerrainSpawnQueue, queue);
        int2 points[terrain_streamers_max];
        const byte points_count = terrain_stream_points(world, query, e, points);
        const double budget_start = get_time_ms();
        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2();
            zox_sys_in_2(StreamDirty);
            zox_sys_in_2(StreamerLevel);
            zox_sys_in_2(StreamPosition);
            zox_sys_out_2(StreamRow);
            zox_sys_out_2(StreamSwept);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_i_2(StreamDirty, dirty);
                zox_sys_i_2(StreamerLevel, level);
                zox_sys_i_2(StreamPosition, stream_position);
                zox_sys_o_2(StreamRow, row);
                zox_sys_o_2(StreamSwept, swept);
                if (zox_get_link(world, it2.entities[j], StreamLink) != e) {
                    continue;
                }
                // a sweep from where it is: when it's moved since its last (StreamSwept), or restreamed (StreamDirty
                //  triggered: a game start, a town placed); waiting on one StreamDirty frame missed some (two_streamers)
                if (!int3_equals(swept->value, stream_position->value) || dirty->value == zox_dirty_trigger) {
                    swept->value = stream_position->value;
                    row->value = 0;
                }
                if (row->value < 0) {
                    continue;
                }
                const int2 stream_position2 = (int2) { stream_position->value.x, stream_position->value.z };
                while (row->value < rows && get_time_ms() - budget_start < zox_terrain_stream_budget_ms) {
                    const int z = stream_position2.y + terrain_stream_row_offset(row->value);
                    for (int x = stream_position2.x - size; x <= stream_position2.x + size; x++) {
                        const int2 tunk_position = (int2) { x, z };
                        const entity tunk = int2_hashmap_get(tunks->value, tunk_position);
                        // NOTE: If tunk doesnt exist, spawn new terrain pillar here!
                        const int distance = int2_distance(stream_position2, tunk_position);
                        const byte added = add_or_set_TerrainSpawnQueue(queue, (TerrainSpawnUpdate) {
                            .tunk = tunk,
                            .level = level->value,
                            .position = tunk_position,
                            .distance = level->value >= 1 ?
                                terrain_stream_distance(tunk_position, distance, points, points_count) : distance,
                            .time = zox_current_time,
                        });
                        if (dbg_log && added) {
                            zox_log("Queueing Update: Tunk [%s] At [%ix%i]", zox_getn(tunk), x, z);
                        }
                    }
                    row->value++;
                }
                if (row->value >= rows) {
                    row->value = -1;
                }
            }
        }
        zox_sys_query_end();
    }
    zox_sys_on_end();
} zoxd_system(terrain_stream_system);

// sets a tunk's new distance on it and its chunks; a new lod also sets their depth and starts the chunks' lod swaps
//  returns 0 if the distance is the same, 1 for only the distance, 2 if its lod changed
static inline byte stream_tunk_distance(ecs* world, entity terrain, entity tunk, const Chunk3Stack* stack, byte new_distance) {
    byte dbg_log = 0;
    RenderDistance* tunk_distance = zox_mut(tunk, RenderDistance);
    if (tunk_distance->value == new_distance) {
        return 0;
    }
    tunk_distance->value = new_distance;
    zox_setv(tunk, RenderDistanceDirty, 1);
    byte new_tunk_render_depth = camera_distance_to_terrain_render_depth(new_distance);
    RenderDepth* tunk_render_depth = zox_mut(tunk, RenderDepth);
    byte tunk_render_depth_dirty = tunk_render_depth->value != new_tunk_render_depth;
    if (tunk_render_depth_dirty) {
        tunk_render_depth->value = new_tunk_render_depth;
    }
    /*if (lod->value == tunk_render_depth) {
        continue;
    }*/
    TunkLod* lod = zox_mut(tunk, TunkLod);
    if (new_tunk_render_depth > lod->value) {
        lod->value = new_tunk_render_depth;
        zox_setv(tunk, GenerateTunk, zox_generate_tunk_start);
        if (dbg_log) {
            zox_log("Tunk Depth [%s] Dist [%i] Depth [%i]",
                zox_getn(tunk),
                new_distance,
                lod->value);
        }
    }
    for (int stack_i = 0; stack_i < stack->length; stack_i++) {
        const int y = stack_i - stack->length / 2;
        entity chunk = stack->value[stack_i];
#ifdef zox_safety_checks
        if (!zox_valid(chunk)) {
            zox_logw("[%s]'s Chunk missing at Y [%i]",
                zox_getn(tunk),
                y);
            continue;
        }
#endif
        zox_setm(chunk, RenderDistance, new_distance);
        zox_setv(chunk, RenderDistanceDirty, zox_dirty_trigger);
        if (!tunk_render_depth_dirty) {
            continue;
        }
        zox_setm(chunk, RenderDepth, new_tunk_render_depth);
        // NOTE: New component
        zox_setv(chunk, ChunkLodDirty, zox_chunk_lod_dirty_start);
        // Stop the active building
        chunk_meshes_stop_building(world, chunk);
        if (dbg_log >= 2) {
            zox_log("   + Chunk [%s] Depth[%i]",
                zox_getn(chunk),
                new_tunk_render_depth);
        }
        // NOTE: Clears the light if depth is set to increase
        if (zox_no_lights) {
            continue;
        }
        byte node_depth = zox_getv(chunk, NodeDepth);
        if (new_tunk_render_depth > node_depth) {
            // NOTE: Causes flicker but needed for fixing lighting in older chunks
            zox_muter(chunk, LightNode, lights);
            lights->value = darklight;
            collapse_LightNode(lights);
            if (y == render_distance_y) {
                zox_add(terrain, GenerateSunlight);
            }
        }
    }
    return tunk_render_depth_dirty ? 2 : 1;
}

// distance only updates (the lod stays) are cheap: applied to every queued tunk at once, outside the budget, so the
//  budget goes to lod changes and spawns (most of a crossing's updates were distance only), doc/done/streaming.md
static inline void stream_queue_distances(ecs* world, entity terrain, TerrainSpawnQueue* queue) {
    for (size_t i = 0; i < queue->count;) {
        const TerrainSpawnUpdate* update = &queue->ptr[i];
        const byte distance = update->distance;
        const entity tunk = update->tunk;
        if (!tunk || distance > terrain_lod_far || !zox_valid(tunk) ||
            camera_distance_to_terrain_render_depth(distance) != zox_getv(tunk, RenderDepth))
        {
            i++;
            continue;
        }
        remove_at_TerrainSpawnQueue(queue, i);
        if (stream_tunk_distance(world, terrain, tunk, zox_get(tunk, Chunk3Stack), distance)) {
            stream_slots_distance++;
        }
    }
}

void terrain_stream_queue_system(iter* it) {
    byte dbg_log = 0;
    // a time budget a frame, so a fast machine streams faster and a slow frame doesn't spike; capped in count too,
    //  since what it starts (generation, meshes) costs more later than spawning does now
    const uint max_process = stream_updates_max_per_frame;
    const double budget_start = get_time_ms();
    int3 size = (int3) {
        terrain_lod_far,
        render_distance_y,
        terrain_lod_far
    };
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_in(BlockScale);
    zox_sys_in(NodeDepth);
    zox_sys_out(TerrainSpawnQueue);
    zox_sys_out(RegionLinks);
    zox_sys_out(TunkLinks);
    zox_sys_out(ChunkLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, seed);
        zox_sys_i(BlockScale, block_scale);
        zox_sys_i(NodeDepth, depth);
        zox_sys_o(TerrainSpawnQueue, queue);
        zox_sys_o(RegionLinks, regions);
        zox_sys_o(TunkLinks, tunks);
        zox_sys_o(ChunkLinks, chunks);
        if (!queue->count) {
            continue;
        }
        if (dbg_log) {
            zox_log(
                "[Stream] Terrain [%i] queue count [%i]",
                e,
                queue->count
            );
        }
        entity realm = zox_get_parent(world, e);
        // the game ending: its realm goes this frame, tunks spawned now were left without it (end_game_cleanup)
        if (game_ending(world, zox_get_link(world, realm, GameLink))) {
            clear_TerrainSpawnQueue(queue);
            continue;
        }
        entity tilemap = zox_get_link(world, e, Tilemap);
        uint processed = 0;
        byte chunk_length = octree_size(depth->value);
        stream_queue_distances(world, e, queue);
        while (queue->count && processed < max_process && get_time_ms() - budget_start < stream_update_ms) {

            // TerrainSpawnUpdate update = remove_TerrainSpawnQueue(queue);
            TerrainSpawnUpdate update = remove_next_TerrainSpawnQueue(queue, zox_current_time, stream_update_aging);

            byte new_distance = update.distance;
            byte is_kill = new_distance > terrain_lod_far;
            // if update
            if (update.tunk) {
                // TODO: Validate if safetychecks
                entity tunk = update.tunk;
                const Chunk3Stack* stack = zox_get(tunk, Chunk3Stack);
                // left: unloaded a little a frame (sys/stream_unload.c); back before it's gone: kept
                if (is_kill) {
                    if (!zox_has(tunk, TunkUnloading)) {
                        zox_add(tunk, TunkUnloading);
                    }
                    continue;
                }
                if (zox_has(tunk, TunkUnloading)) {
                    zox_remove(tunk, TunkUnloading);
                }
                const byte changed = stream_tunk_distance(world, e, tunk, stack, new_distance);
                if (!changed) {
                    continue;
                }
                if (changed == 2) {
                    stream_slots_lod++;
                } else {
                    stream_slots_distance++;
                }
                processed++;
                continue;
            }
            // TODO: Move this into queue system later
            if (is_kill) {
                continue;
            }
            // if spawn
            int2 region_position = tunk_position_to_region_position(update.position);
            // NOTE: Spawn our region if it doesn't exist!
            entity region = int2_hashmap_get(regions->value, region_position);
            if (!zox_valid(region)) {
                // Generate Seed from Terrain Seed
                lint region_seed =
                    position_seed2(
                        seed->value,
                        region_position);
                int2 block_position =
                    region_position_to_block_position(
                        region_position,
                        depth->value);
                int2 block_size = (int2) {
                    region_dividor * chunk_length,
                    region_dividor * chunk_length
                };
                region = spawn_region(
                    world,
                    prefab_region,
                    e,
                    region_seed,
                    region_position,
                    int2_one,
                    block_position,
                    block_size);
                int2_hashmap_add(
                    regions->value,
                    region_position,
                    region);
                if (dbg_log) {
                    zox_log("New Region [%ix%i] Spawned",
                        region_position.x,
                        region_position.y);
                }
            }
            if (update.level < 1) {
                processed++;
                continue;
            }
            // int new_distance = int2_distance(stream_position2, position);
            entity tunk = int2_hashmap_get(
                tunks->value,
                update.position);
            // NOTE: If tunk doesnt exist, spawn new terrain pillar here!
            if (zox_valid(tunk)) {
                continue;
            }
            byte new_depth = camera_distance_to_terrain_render_depth(new_distance);
            tunk = spawn_tunk(
                world,
                prefab_tunk2,
                e,
                region,
                update.position,
                new_distance,
                new_depth);
            int2_hashmap_add(tunks->value, update.position, tunk);
            if (dbg_log) {
                zox_log("New Tunk: [%ix%i] dist [%i]",
                    update.position.x,
                    update.position.y,
                    update.distance);
            }
            // tunks only (towns/dbg/maps_dump.c, --maps): their maps generate, no chunks
            if (zox_terrain_no_chunks) {
                stream_slots_spawn++;
                processed++;
                continue;
            }
            // Spawn chunks per Tunk, if new!
            Chunk3Stack stack = (Chunk3Stack) { 0 };
            initialize_Chunk3Stack(&stack, size.y * 2 + 1);
            int stack_i = 0;
            for (int y = - size.y; y <= size.y; y++, stack_i++) {
                int3 chunk_position = (int3) {
                    update.position.x,
                    y,
                    update.position.y
                };
                entity chunk = spawn_chunk3_textured(
                    world,
                    prefab_chunk_terrain,
                    realm,
                    e,
                    tilemap,
                    seed->value,
                    chunk_position,
                    depth->value,
                    block_scale->value,
                    update.distance,
                    new_depth);
                zox_link(world, chunk, TunkLink, tunk);
                if (y == render_distance_y) {
                    zox_add(chunk, SunnyChunk);
                } else if (y == -render_distance_y) {
                    zox_add(chunk, BottomChunk);
                }
                int3_hashmap_add(chunks->value, chunk_position, chunk);
                stack.value[stack_i] = chunk;
                if (dbg_log) {
                    zox_log("New Chunk: [%ix%ix%i] dist [%i]",
                        chunk_position.x, chunk_position.y, chunk_position.z, update.distance);
                }
            }
            zox_set_ptr(tunk, Chunk3Stack, stack);  // copied in
            dispose_Chunk3Stack(&stack);
            stream_slots_spawn++;
            processed++;
        }
    }
    zox_sys_on_end();
} zoxd_system(terrain_stream_queue_system);
