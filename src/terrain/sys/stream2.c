// TODO Mark neighbors of spawning chunks as Dirty (NeighborsDirty)

// NOTE: Uses hash queue to add new positions to spawning
zox_sys2(TerrainStreamSystem) {
    byte dbg_log = 0;
    int3 size = (int3) { terrain_lod_far, render_distance_y, terrain_lod_far };
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TunkLinks);
    zox_sys_out(TerrainSpawnQueue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TunkLinks, tunks);
        zox_sys_o(TerrainSpawnQueue, queue);
        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2();
            zox_sys_in_2(StreamDirty);
            zox_sys_in_2(StreamerLevel);
            zox_sys_in_2(StreamLink);
            zox_sys_in_2(StreamPosition);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_i_2(StreamDirty, dirty);
                zox_sys_i_2(StreamerLevel, level);
                zox_sys_i_2(StreamLink, terrain);
                zox_sys_i_2(StreamPosition, stream_position);
                // NOTE: For some reason this was failing for StreamDirty flags
                if (!dirty->value) {
                    continue;
                }
                if (terrain->value != e) {
                    continue;
                }
                int2 stream_position2 = (int2) { stream_position->value.x, stream_position->value.z };
                int3 position = int3_zero;
                for (position.x = stream_position->value.x - size.x; position.x <= stream_position->value.x + size.x; position.x++) {
                    for (position.z = stream_position->value.z - size.z; position.z <= stream_position->value.z + size.z; position.z++) {
                        int2 tunk_position = (int2) { position.x, position.z };
                        entity tunk = int2_hashmap_get(tunks->value, tunk_position);
                        // NOTE: If tunk doesnt exist, spawn new terrain pillar here!
                        if (!zox_valid(tunk)) {
                            byte added = add_TerrainSpawnQueue(queue,
                                (TerrainSpawnUpdate) {
                                    .level = level->value,
                                    .position = tunk_position,
                                    .distance =  int2_distance(stream_position2, tunk_position)
                                });
                            if (dbg_log && added) {
                                zox_log("Queueing Spawn Position [%ix%i]",
                                    tunk_position.x, tunk_position.y);
                            }
                        }
                    }
                }
            }
        }
        zox_sys_query_end();
    }
} zox_sys_end(TerrainStreamSystem);

zox_sys2(TerrainQueueSystem) {
    byte dbg_log = 0;
    uint max_process = 1; // NOTE How to scale this better per cpu power
    int3 size = (int3) { terrain_lod_far, render_distance_y, terrain_lod_far };
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
        entity tilemap = zox_getv(e, TilemapLink);
        // entity realm = zox_getv(e, RealmLink);
        uint processed = 0;
        byte chunk_length = octree_size(depth->value);
        while (queue->count && processed < max_process) {
            TerrainSpawnUpdate update = remove_TerrainSpawnQueue(queue);
            int2 region_position = tunk_position_to_region_position(update.position);
            // NOTE: Spawn our region if it doesn't exist!
            entity region = int2_hashmap_get(regions->value, region_position);
            if (!zox_valid(region)) {
                // Generate Seed from Terrain Seed
                lint region_seed = position_seed2(seed->value, region_position);
                int2 block_position = region_position_to_block_position(region_position, depth->value);
                int2 block_size = (int2) { region_dividor * chunk_length, region_dividor * chunk_length };
                region = spawn_region(world, prefab_region, e, region_seed, region_position, int2_one, block_position, block_size);
                int2_hashmap_add(regions->value, region_position, region);
                if (dbg_log) {
                    zox_log("New Region [%ix%i] Spawned", region_position.x, region_position.y);
                }
            }
            if (update.level < 1) {
                continue;
            }
            // int new_distance = int2_distance(stream_position2, position);
            entity tunk = int2_hashmap_get(tunks->value, update.position);
            // NOTE: If tunk doesnt exist, spawn new terrain pillar here!
            if (zox_valid(tunk)) {
                continue;
            }
            byte new_depth = camera_distance_to_terrain_render_depth(update.distance);
            tunk = spawn_tunk(
                world,
                prefab_tunk2,
                e,
                region,
                update.position,
                update.distance,
                new_depth);
            int2_hashmap_add(tunks->value, update.position, tunk);
            if (dbg_log) {
                zox_log("New Tunk: [%ix%i] dist [%i]",
                    update.position.x, update.position.y, update.distance);
            }
            // Spawn chunks per Tunk, if new!
            Chunk3Stack stack = (Chunk3Stack) { 0 };
            byte stack_i = 0;
            for (int y = - size.y; y <= size.y; y++, stack_i++) {
                int3 chunk_position = (int3) {
                    update.position.x,
                    y,
                    update.position.y
                };
                entity chunk = spawn_terrain_chunk(
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
                int3_hashmap_add(chunks->value, chunk_position, chunk);
                zox_link(world, chunk, Tunk, tunk);
                if (y == render_distance_y) {
                    zox_add(chunk, SunnyChunk);
                } else if (y == -render_distance_y) {
                    zox_add(chunk, BottomChunk);
                }
                if (dbg_log) {
                    zox_log("New Chunk: [%ix%ix%i] dist [%i]",
                        chunk_position.x, chunk_position.y, chunk_position.z, update.distance);
                }
                stack.value[stack_i] = chunk;
            }
            zox_set_ptr(tunk, Chunk3Stack, stack);
            processed++;
        }
    }
} zox_sys_end(TerrainQueueSystem);
