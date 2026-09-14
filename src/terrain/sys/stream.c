// TODO Mark neighbors of spawning chunks as Dirty (NeighborsDirty)

// NOTE: Uses hash queue to add new positions to spawning
void terrain_stream_system(iter* it) {
    byte dbg_log = 0;
    int3 size = (int3) {
        terrain_lod_far,
        render_distance_y,
        terrain_lod_far
    };
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
                int2 stream_position2 = (int2) {
                    stream_position->value.x,
                    stream_position->value.z
                };
                int3 position = int3_zero;
                for (position.x = stream_position->value.x - size.x; position.x <= stream_position->value.x + size.x; position.x++)
                for (position.z = stream_position->value.z - size.z; position.z <= stream_position->value.z + size.z; position.z++)
                {
                    int2 tunk_position = (int2) {
                        position.x,
                        position.z
                    };
                    entity tunk = int2_hashmap_get(tunks->value, tunk_position);
                    // NOTE: If tunk doesnt exist, spawn new terrain pillar here!
                    byte added = add_or_set_TerrainSpawnQueue(queue,
                        (TerrainSpawnUpdate) {
                            .tunk = tunk,
                            .level = level->value,
                            .position = tunk_position,
                            .distance = int2_distance(
                                stream_position2,
                                tunk_position),
                            .time = zox_current_time,
                        });
                    if (dbg_log && added) {
                        zox_log("Queueing Update: Tunk [%s] At [%ix%i]",
                            zox_getn(tunk),
                            tunk_position.x, tunk_position.y);
                    }
                }
            }
        }
        zox_sys_query_end();
    }
    zox_sys_on_end();
} zoxd_system(terrain_stream_system);

void terrain_stream_queue_system(iter* it) {
    byte dbg_log = 0;
    uint max_process = stream_update_rate; // NOTE How to scale this better per cpu power
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
        entity tilemap = zox_get_link(world, e, Tilemap);
        uint processed = 0;
        byte chunk_length = octree_size(depth->value);
        while (queue->count && processed < max_process) {
            TerrainSpawnUpdate update = remove_TerrainSpawnQueue(queue);
            int2 position2 = update.position;
            byte new_distance = update.distance;
            byte is_kill = new_distance > terrain_lod_far;
            // if update
            if (update.tunk) {
                // TODO: Validate if safetychecks
                entity tunk = update.tunk;
                const Chunk3Stack* stack = zox_get(tunk, Chunk3Stack);
                if (is_kill) {
                    int k = 0;
                    for (int j = -render_distance_y; j <= render_distance_y; j++, k++) {
                        entity chunk = stack->value[k];
                        if (!zox_valid(chunk)) {
                            continue;
                        }
                        int3 position3 = (int3) {
                            position2.x,
                            j,
                            position2.y
                        };
                        int3_hashmap_remove(chunks->value, position3);
                        zox_delete(chunk);
                    }
                    int2_hashmap_remove(tunks->value, position2);
                    zox_delete(tunk);
                    continue;
                }
                RenderDistance* tunk_distance = zox_mut(tunk, RenderDistance);
                if (tunk_distance->value == new_distance) {
                    continue;
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
                byte stack_i = 0;
                for (short y = - render_distance_y; y <= render_distance_y; y++, stack_i++) {
                    entity chunk = stack->value[stack_i];
#ifdef zox_safety_checks
                    if (!zox_valid(chunk)) {
                        zox_logw("[%s]'s Chunk missing at Y [%i]",
                            zox_getn(tunk),
                            y);
                        continue;
                    }
#endif
                    /*byte old_distance3 = zox_getv(chunk, RenderDistance);
                    if (old_distance3 == new_distance) {
                        continue;
                    }*/
                    zox_setm(chunk, RenderDistance, new_distance);
                    zox_setv(chunk, RenderDistanceDirty, zox_dirty_trigger);
                    // If same depth, continue
                    // byte old_depth = zox_getv(chunk, RenderDepth);
                    if (!tunk_render_depth_dirty) {
                        continue;
                    }
                    zox_setm(chunk, RenderDepth, new_tunk_render_depth);
                    // NOTE: New component
                    zox_setv(chunk, ChunkLodDirty, zox_chunk_lod_dirty_start);
                    // Stop the active building
                    // entity active_mesh = zox_getv(chunk, ActiveMesh);
                    entity active_mesh = zox_get_link(world, chunk, ActiveMesh);
                    if (zox_valid(active_mesh) &&
                        !zox_has(active_mesh, BuildDisabled))
                    {
                        zox_add(active_mesh, BuildDisabled);
                    }
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
                            zox_add(e, GenerateSunlight);
                        }
                    }
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
                lint region_seed = position_seed2(seed->value, region_position);
                int2 block_position = region_position_to_block_position(region_position, depth->value);
                int2 block_size = (int2) { region_dividor * chunk_length, region_dividor * chunk_length };
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
                int3_hashmap_add(chunks->value, chunk_position, chunk);
                zox_link(world, chunk, TunkLink, tunk);
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
    zox_sys_on_end();
} zoxd_system(terrain_stream_queue_system);
