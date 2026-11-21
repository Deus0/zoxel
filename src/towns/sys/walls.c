// Town walls, gates, roads, street lamps and highways from the town map, homes from fun/home.c
//  - a street lamp: its base on the ground, two fillers over it (items/fun/multi_block.c), its head lights the street
//  - highways paved on the ground, or at the sea's top over water (a bridge)
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
    zox_sys_out(BlockDatas);
    const int sea = terrain_sea_height * render_distance_y;
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_o(GenerateChunk, generate);
        zox_sys_o(VoxelNode, voctree);
        BlockDatas* datas = BlockDatas_ ? &BlockDatas_[i] : NULL;
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
        // a glow stone on each home, lit with the chunk's other emitters (lights3/sys/emitters.c)
        const entity glow = zox_get_child_by_id(world, realm, zox_id(BlockGlowStone));
        const byte glow_id = zox_valid(glow) ? zox_getv(glow, BlockIndex) : 0;
        // furniture and street lamps, sized blocks (items/fun/multi_block.c), only at full detail with block data
        const entity door = zox_get_child_by_id(world, realm, zox_id(BlockDoor));
        const entity bed = zox_get_child_by_id(world, realm, zox_id(BlockBed));
        const entity filler = zox_get_child_by_id(world, realm, zox_id(BlockFiller));
        const entity lamp = zox_get_child_by_id(world, realm, zox_id(BlockLamp));
        byte lamp_id = datas && zox_valid(lamp) ? zox_getv(lamp, BlockIndex) : 0;
        byte length = octree_size(build_depth);
        int3 chunk_block_position = chunk_position_to_block_position(cposition->value, terrain_depth);
        int2 map_size = int2_single(length); // max_chunk_length);
        byte hmultiplier = octree_size(terrain_depth - build_depth);
        // the homes crossing this chunk, for their roofs
        TownHome homes[town_homes_max];
        const byte homes_count = town_homes_in(world, zox_get_link(world, tunk, RegionLink),
            (int2) { chunk_block_position.x, chunk_block_position.z },
            (int2) { chunk_block_position.x + length * hmultiplier - 1, chunk_block_position.z + length * hmultiplier - 1 }, homes);
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
        HomeBlocks blocks = { .wall = bricks_id, .floor = bricks_id, .roof = wood_id, .glow = glow_id,
            .door = zox_valid(door) ? zox_getv(door, BlockIndex) : 0, .bed = zox_valid(bed) ? zox_getv(bed, BlockIndex) : 0,
            .filler = zox_valid(filler) ? zox_getv(filler, BlockIndex) : 0, .door_filler = block_filler_voxel(world, realm, door) };
        if (!blocks.filler) {
            lamp_id = 0;
        }
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
                if (town_value == zox_town_type_road) {
                    // its roads and square paved, the rest of its ground left as it is
                    town_set_block(voctree, build_depth, &position, height, chunk_y, hmultiplier, length, road_id);
                } else if (town_value == zox_town_type_highway) {
                    town_set_block(voctree, build_depth, &position, int_max(height, sea), chunk_y, hmultiplier, length, road_id);
                } else if (town_value == zox_town_type_lamp) {
                    // its base and fillers, each with its block data (its direction, their offsets down to it)
                    if (!lamp_id || hmultiplier != 1) {
                        continue;
                    }
                    for (int h = 0; h < 3; h++) {
                        if (town_set_block(voctree, build_depth, &position, height + 1 + h, chunk_y, hmultiplier, length, h ? blocks.filler : lamp_id)) {
                            set_block_data(datas, position, h ? block_offset_encode((int3) { 0, -h, 0 }) : 1);
                        }
                    }
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
            }
        }
        // homes crossing this chunk, from their boxes (fun/home.c)
        for (byte k = 0; k < homes_count; k++) {
            Building building;
            int3 glow_block;
            build_home(&building, &homes[k], &blocks, &glow_block);
            build_in_chunk(voctree, datas, build_depth, chunk_block_position, hmultiplier, length, &building);
        }
        zox_octree_lock_end(write_lock);
        generate->value = zox_generate_terrain_sunlight;
    }
    zox_sys_on_end();
} zoxd_system(town_walls_system);
