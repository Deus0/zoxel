// NOTE: Just spawns chunks from here
zox_sys2(TerrainStreamSystem) {
    // TODO: Queue Position Checks to work over frames
    // TODO: Support for multiple streamers
    // TODO: Cache Streamers (like Billboard System)
    byte dbg_log = 0;
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_in(BlockScale);
    zox_sys_in(NodeDepth);
    zox_sys_out(RegionLinks);
    zox_sys_out(TunkLinks);
    zox_sys_out(ChunkLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, seed);
        zox_sys_i(BlockScale, block_scale);
        zox_sys_i(NodeDepth, depth);
        zox_sys_o(RegionLinks, regions);
        zox_sys_o(TunkLinks, tunks);
        zox_sys_o(ChunkLinks, chunks);
        // NOTE: For all streamers, check chunks exist for their stream sizes
        // TODO: Add a list here of new positions then spawn them after streamers check, keep closest distance, due to multiple stream points
        byte chunk_length = octree_size(depth->value);
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
                // if (dirty->value != zox_dirty_active) {
                if (!dirty->value) {
                    continue;
                }
                if (terrain->value != e) {
                    continue;
                }
                // Shows 1, 2, 3
                // zox_log("dirty->value: %i", dirty->value);
                int2 stream_position2 = (int2) { stream_position->value.x, stream_position->value.z };
                int3 size = (int3) { terrain_lod_far, render_distance_y, terrain_lod_far };
                int3 position = int3_zero;
                for (position.x = stream_position->value.x - size.x; position.x <= stream_position->value.x + size.x; position.x++)
                for (position.z = stream_position->value.z - size.z; position.z <= stream_position->value.z + size.z; position.z++) {
                    int2 tunk_position = (int2) { position.x, position.z };
                    int2 region_position = tunk_position_to_region_position(tunk_position);
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
                    if (level->value < 1) {
                        continue;
                    }
                    int new_distance = int2_distance(stream_position2, tunk_position);
                    entity tunk = int2_hashmap_get(tunks->value, tunk_position);
                    // NOTE: If tunk doesnt exist, spawn new terrain pillar here!
                    if (!zox_valid(tunk)) {
                        byte new_depth = camera_distance_to_terrain_render_depth(new_distance);
                        tunk = spawn_tunk(world, prefab_tunk2, e, region, tunk_position, new_distance, new_depth);
                        int2_hashmap_add(tunks->value, tunk_position, tunk);
                        // Spawn chunks per Tunk, if new!
                        Chunk3Stack stack = (Chunk3Stack) { 0 };
                        byte stack_i = 0;
                        for (position.y = - size.y; position.y <= size.y; position.y++, stack_i++) {
                            // stack.value[stack_i] = int3_hashmap_get(chunks->value, position);
                            // if (zox_valid(stack.value[stack_i])) continue;
                            entity chunk = spawn_terrain_chunk(world, prefab_chunk_terrain, e, position, depth->value, block_scale->value, new_distance, new_depth);
                            int3_hashmap_add(chunks->value, position, chunk);
                            zox_set(chunk, TunkLink, { tunk });
                            if (position.y == render_distance_y) {
                                zox_add_tag(chunk, SunnyChunk);
                            } else if (position.y == -render_distance_y) {
                                zox_add_tag(chunk, BottomChunk);
                            }
                            if (dbg_log) {
                                zox_log("New Chunk: [%ix%ix%i] dist [%i]", position.x, position.y, position.z, new_distance);
                            }
                            stack.value[stack_i] = chunk;
                        }
                        zox_set_ptr(tunk, Chunk3Stack, stack);
                    } /*else {
                        // NOTE: Handled by tunk loding
                        if (zox_tunk_lod_system) {
                            continue;
                        }
                        // Test lag issues
                        // continue;
                        // NOTE: If already exist, update lods!
                        // NOTE: Sets Tunk Render Distance too!
                        byte old_distance2 = zox_getv(tunk, RenderDistance);
                        if (old_distance2 == new_distance) {
                            continue;
                        }
                        zox_setm(tunk, RenderDistance, new_distance);
                        zox_setm(tunk, RenderDistanceDirty, zox_dirty_trigger);
                        byte old_tunk_lod = zox_getv(tunk, TunkLod);
                        // NOTE: If LOD Increases, we Regenerate Tunk! Only uses Lod, no RenderLods since no Renders!
                        byte new_depth = terrain_depth; //  camera_distance_to_terrain_render_depth(new_distance);
                        if (new_depth > old_tunk_lod) {
                            zox_setm(tunk, TunkLod, new_depth);
                            zox_setm(tunk, GenerateTunk, zox_generate_tunk_start);
                        }
                        if (zox_terrain_chunk_lod_system) {
                            continue;
                        }
                        zox_geter(tunk, Chunk3Stack, stack);
                        byte stack_i = 0;
                        for (position.y = - size.y; position.y <= size.y; position.y++, stack_i++) {
                            entity chunk = stack->value[stack_i];
#ifdef zox_safety_checks
                            if (!zox_valid(chunk)) {
                                zox_logw("Chunk missing at [%ix%ix%i]", position.x, position.y, position.z);
                                continue;
                            }
#endif
                            byte old_distance3 = zox_getv(chunk, RenderDistance);
                            if (old_distance3 == new_distance) {
                                continue;
                            }
                            zox_setm(chunk, RenderDistance, new_distance);
                            zox_setm(chunk, RenderDistanceDirty, zox_dirty_trigger);
                            byte old_depth = zox_getv(chunk, RenderDepth);
                            if (old_depth == new_depth) {
                                continue;
                            }
                            zox_setm(chunk, RenderDepth, new_depth);
                            zox_setm(chunk, RenderDepthDirty, zox_dirty_trigger);
                            // zox_setm(chunk, Busy, 1);
                            if (dbg_log) {
                                zox_log("[Stream] Chunk Depth Updated [%s]:[%i]", zox_get_name(chunk), new_depth);
                            }
                            // NOTE: Started to be busy! TODO: Move this to generate starts
                            byte node_depth = zox_getv(chunk, NodeDepth);
                            if (new_depth > node_depth) {
                                // NOTE: Clears the light if depth is set to increase
                                zox_muter(chunk, LightNode, lights);
                                lights->value = darklight;
                                collapse_LightNode(lights);
                                if (position.y == render_distance_y) {
                                    zox_set(chunk, GenerateLights, { zox_generate_lights_sunlight });
                                }
                            }
                        }
                    }*/
                }
            }
        }
        zox_sys_query_end();
    }
} zox_sys_end(TerrainStreamSystem);

// TODO: Check all configurations on different issues this might bring... if it comes up
// NOTE: No need atm for extra flooding, seems that it's just working as  sunlights just readds the floodfill as it fills
// zox_set(chunk, RefreshLights, { zox_refresh_lights });
