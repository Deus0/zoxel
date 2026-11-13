// Town walls, gates and roads from the town map, homes from fun/home.c
// NOTE: Uses Height Map + Town Map to spawn Town Walls in chunks
void town_walls_system(iter* it) {
    zox_sys_on_begin();
    // TODO: Link the Chunk to Town and use hte town data
    // TODO: get the Home from the Town based on position
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkPosition);
    zox_sys_out(GenerateChunk);
    zox_sys_out(VoxelNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_o(GenerateChunk, generate);
        zox_sys_o(VoxelNode, voctree);
        if (generate->value != zox_generate_terrain_towns) {
            continue;
        }
        if (zox_disable_towns) {
            generate->value = zox_generate_terrain_sunlight;
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        entity realm = zox_get_parent(world, terrain);
#ifdef zox_safety_checks
        if (!zox_valid(realm)) {
            zox_loge("Realm invalid");
            continue;
        }
#endif
        const BiomeLinks* realm_biomes = zox_get(realm, BiomeLinks);
#ifdef zox_safety_checks
        if (!realm_biomes->length) {
            zox_loge("No Biomes on Realm");
            continue;
        }
#endif
        entity tunk = zox_get_link(world, e, TunkLink);
#ifdef zox_safety_checks
        if (!zox_valid(tunk)) {
            zox_loge("[Towns] Invalid [Tunk] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
#endif
        byte tunk_lod = zox_getv(tunk, TunkLod);
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        byte build_depth = depth->value;
        if (depth->value != tunk_lod) {
            build_depth = tunk_lod;
            zox_loge("[Towns] Chunk Depth Invalid [%i] - [%i]",
                depth->value,
                tunk_lod);
            // continue;
        }
        entity wall = zox_get_child_by_id(
            world,
            realm,
            zox_id(BlockBricks));
#ifdef zox_safety_checks
        if (!zox_valid(wall)) {
            zox_loge("No wall for town..");
            continue;
        }
#endif
        byte bricks_id = zox_getv(wall, BlockIndex);
#ifdef zox_safety_checks
        if (!bricks_id) {
            zox_loge("Bricks ID Invalid");
            continue;
        }
#endif
        // a glow stone on each home (lights it, sys light seeds below)
        const entity glow = zox_get_child_by_id(world, realm, zox_id(BlockGlowStone));
        const byte glow_id = zox_valid(glow) ? zox_getv(glow, BlockIndex) : 0;
        const byte glow_light = zox_valid(glow) ? zox_getv(glow, BlockLight) : 0;
        byte length = octree_size(build_depth);
        int3 chunk_block_position = chunk_position_to_block_position(cposition->value, terrain_depth);
        int2 map_size = int2_single(length); // max_chunk_length);
        byte hmultiplier = octree_size(terrain_depth - build_depth);
        // the homes crossing this chunk, for their roofs
        TownHome homes[town_homes_max];
        const byte homes_count = town_homes_in(world, zox_get_link(world, tunk, RegionLink),
            (int2) { chunk_block_position.x, chunk_block_position.z },
            (int2) { chunk_block_position.x + length * hmultiplier - 1, chunk_block_position.z + length * hmultiplier - 1 }, homes);
        byte3 glow_positions[town_homes_max];
        byte glows = 0;
        // int chunk_position_y = chunk_block_position.y;
        byte gate_height = 6; // rand_range(4, 8);
        byte wall_height = 8; // rand_range(4, 8);
        const BiomeMap* biome_map = zox_get(tunk, BiomeMap);
        const HeightMap* height_map = zox_get(tunk, HeightMap);
        const TownMap* town_map = zox_get(tunk, TownMap);
#ifdef zox_safety_checks
        if (!town_map->length) {
            zox_loge("Invalid [Tunk] [town_map] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
#endif
        // its octree locked while written, readers can run alongside (thread_safety.md)
        spinlock* write_lock = zox_octree_lock(world, e);
        zox_octree_lock_begin(write_lock);
        entity biome = 0;
        byte road_id = 0;
        byte wood_id = bricks_id;
        HomeBlocks blocks = { .wall = bricks_id, .floor = bricks_id, .roof = wood_id, .glow = glow_id };
        byte3 position;
        for (position.x = 0; position.x < length; position.x++) {
            for (position.z = 0; position.z < length; position.z++) {
                int2 map_position = (int2) { position.x, position.z };
                int index = int2_array_index(map_position, map_size);
#ifdef zox_safety_checks
                if (index >= town_map->length) {
                    zox_loge("Town: Map Index OOB [%i] : [%i].. Pos [%ix%i] Size [%ix%i]", index, town_map->length, map_position.x, map_position.y, map_size.x, map_size.y);
                    continue;
                }
#endif
                byte biome_id = !zox_disable_biomes ?
                    biome_map->value[index] :
                    0;
                // NOTE: Checks if outer bounds to determine if on top of world
#ifdef zox_safety_checks
                if (biome_id >= realm_biomes->length) {
                    zox_loge("Biome ID OOB [%i] of [%i]", biome_id, realm_biomes->length);
                    continue;
                }
#endif
                entity new_biome = realm_biomes->value[biome_id];
#ifdef zox_safety_checks
                if (!zox_valid(new_biome)) {
                    zox_loge("Biome is invalid [%i]", biome_id);
                    continue;
                }
#endif
                // NOTE: Updates our cache of our biome
                if (biome != new_biome) {
                    biome = new_biome;
                    entity road = zox_get_child_by_id(world, biome, zox_id(BlockRoad));
                    road_id = zox_valid(road) ? zox_getv(road, BlockIndex) : 0;
                    const entity wood = zox_get_child_by_id(world, biome, zox_id(BlockWood));
                    wood_id = zox_valid(wood) ? zox_getv(wood, BlockIndex) : bricks_id;
                    blocks.roof = wood_id;
                }
                byte height = height_map->value[index];
                byte town_value = town_map->value[index];
                if (!town_value) {
                    continue;
                }
                if (dbg_log) {
                    zox_log("Placing Town Wall at [%ix%i]", chunk_block_position.x + position.x, chunk_block_position.z + position.z);
                }
                const int chunk_y = chunk_block_position.y;
                if (town_value == zox_town_type_zone) {
                    // roads: the town's ground
                    town_set_block(voctree, build_depth, &position, height, chunk_y, hmultiplier, length, road_id);
                } else if (town_value == zox_town_type_wall || town_value == zox_town_type_wall_edge || town_value == zox_town_type_wall_spike) {
                    // walls, their edges a block higher, spikes two
                    const int top = wall_height + (town_value == zox_town_type_wall_edge) + (town_value == zox_town_type_wall_spike) * 2;
                    for (int h = 1; h <= top; h++) {
                        town_set_block(voctree, build_depth, &position, height + h, chunk_y, hmultiplier, length, bricks_id);
                    }
                } else if (town_value == zox_town_type_gate) {
                    // the gate: a gap under its wall
                    for (int h = gate_height; h <= wall_height; h++) {
                        town_set_block(voctree, build_depth, &position, height + h, chunk_y, hmultiplier, length, bricks_id);
                    }
                }
                // homes: their floors, walls, door lintels and roofs (fun/home.c)
                if (town_value == zox_town_type_home || town_value == zox_town_type_home_door || town_value == zox_town_type_home_wall) {
                    const int2 g = {
                        chunk_block_position.x + position.x * hmultiplier,
                        chunk_block_position.z + position.z * hmultiplier
                    };
                    const TownHome* home = town_home_at(homes, homes_count, g);
                    if (home && home_build_column(voctree, build_depth, &position, chunk_y, hmultiplier, length,
                        home, &blocks, g, height) && glows < town_homes_max) {
                        glow_positions[glows++] = position;
                    }
                }
            }
        }
        zox_octree_lock_end(write_lock);
        // glow stones light the air on their sides (solid, the flood skips their own voxel), on the light queue of the
        //  chunk each side is in (a glow stone on a chunk's edge lights into its neighbour, as the flood does)
        const ChunkNeighbors* glow_neighbors = glows && glow_light ? zox_get(e, ChunkNeighbors) : NULL;
        for (byte g = 0; g < glows && glow_light; g++) {
            for (byte d = 0; d < 6; d++) {
                const sbyte3 o = neighbor_offsets[d];
                int3 p = { glow_positions[g].x + o.x, glow_positions[g].y + o.y, glow_positions[g].z + o.z };
                const byte outside = p.x < 0 || p.y < 0 || p.z < 0 || p.x >= length || p.y >= length || p.z >= length;
                const entity chunk = outside ? (glow_neighbors ? glow_neighbors->value[d] : 0) : e;
                if (!zox_valid(chunk) || !zox_has(chunk, LightQueue)) {
                    continue;
                }
                p = (int3) { (p.x + length) % length, (p.y + length) % length, (p.z + length) % length };
                LightQueue* light_queue = zox_getm(chunk, LightQueue);
                spin_lock(&light_queue->lock);
                a_LightQueue(light_queue, (LightUpdate) {
                    .pos = int3_to_byte3(p), .depth = build_depth, .distance = light_propogation_distance,
                    .light = glow_light > light_air_decay ? glow_light - light_air_decay : darklight });
                spin_unlock(&light_queue->lock);
            }
        }
        generate->value = zox_generate_terrain_sunlight;
    }
    zox_sys_on_end();
} zoxd_system(town_walls_system);
