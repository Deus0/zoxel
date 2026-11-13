// Home roofs, by each home's HomeStyle: flat, or peaked (rising a block a column in from its long sides to a ridge,
//  gable walls at its ends), in its biome's wood, a glow stone set into its roof in the middle (lights inside, shows outside)
#define town_homes_max 64

typedef struct {
    int2 center;
    byte2 half;     // its size / 2, its walls at center +- half (towns/sys/maps.c)
    byte style;     // 0 flat, 1 peaked
} TownHome;

// the homes of a chunk's region whose columns cross the chunk (x from, to inclusive, z too)
static inline byte town_homes_in(ecs* world, entity region, int2 from, int2 to, TownHome* homes) {
    byte count = 0;
    entity towns[zox_children_capacity];
    const uint towns_count = zox_valid(region) ? zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town)) : 0;
    for (uint t = 0; t < towns_count; t++) {
        entity children[zox_children_capacity];
        const uint homes_count = zox_get_children_by_id(world, towns[t], children, zox_children_capacity, zox_id(Home));
        for (uint h = 0; h < homes_count && count < town_homes_max; h++) {
            const int2 c = zox_getv(children[h], BlockPosition2);
            const byte2 s = zox_getv(children[h], HomeSize);
            const byte2 half = { s.x / 2, s.y / 2 };
            if (c.x + half.x < from.x || c.x - half.x > to.x || c.y + half.y < from.y || c.y - half.y > to.y) {
                continue;
            }
            homes[count++] = (TownHome) { c, half, zox_getv(children[h], HomeStyle) };
        }
    }
    return count;
}

// the home a column is in, NULL if none
static inline const TownHome* town_home_at(const TownHome* homes, byte count, int2 g) {
    for (byte i = 0; i < count; i++) {
        const TownHome* h = &homes[i];
        if (g.x >= h->center.x - h->half.x && g.x <= h->center.x + h->half.x &&
            g.y >= h->center.y - h->half.y && g.y <= h->center.y + h->half.y) {
            return h;
        }
    }
    return NULL;
}

// blocks a roof rises over its walls at a column: 0 flat, peaked a block a column in from its long sides
static inline int home_roof_rise(const TownHome* h, int2 g) {
    if (!h->style) {
        return 0;
    }
    // the ridge runs along its longer side, it slopes across the shorter
    const byte along_x = h->half.x >= h->half.y;
    const int across = along_x ? g.y : g.x;
    const int center = along_x ? h->center.y : h->center.x;
    const int half = along_x ? h->half.y : h->half.x;
    const int in_low = across - (center - half), in_high = (center + half) - across;
    return in_low < in_high ? in_low : in_high;
}

// a peaked roof's end walls, filled up to the roof (a triangle)
static inline byte home_is_gable(const TownHome* h, int2 g) {
    if (!h->style) {
        return 0;
    }
    const byte along_x = h->half.x >= h->half.y;
    return along_x ?
        (g.x == h->center.x - h->half.x || g.x == h->center.x + h->half.x) :
        (g.y == h->center.y - h->half.y || g.y == h->center.y + h->half.y);
}

// sets a block in a chunk at a world height, if it's in this chunk
static inline byte town_set_block(VoxelNode* voctree, byte depth, byte3* position, int global_y, int chunk_y, byte hmultiplier, byte length, byte value) {
    if (global_y < chunk_y) {
        return 0;
    }
    const int y = (global_y - chunk_y) / hmultiplier;
    if (y >= length) {
        return 0;
    }
    position->y = y;
    set_clean_VoxelNode(voctree, depth, *position, value);
    return 1;
}

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
        // Home Placeholders
        byte home_wall_id = bricks_id;
        byte home_floor_id = bricks_id;
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
        byte home_height = 6; // rand_range(4, 8);
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
                }
                byte height = height_map->value[index];
                byte town_value = town_map->value[index];
                if (!town_value) {
                    continue;
                }
                if (dbg_log) {
                    zox_log("Placing Town Wall at [%ix%i]", chunk_block_position.x + position.x, chunk_block_position.z + position.z);
                }
                // byte wall_height = zox_getv(town, Height);
                if (town_value == zox_town_type_zone) {
                    int global_y = height;
                    if (global_y >= chunk_block_position.y) {
                        position.y = (global_y - chunk_block_position.y) / hmultiplier;
                        if (position.y >= 0 && position.y < length) {
                            set_clean_VoxelNode(
                                voctree,
                                build_depth,
                                position,
                                road_id);
                        }
                    }
                } else if (town_value == zox_town_type_wall || town_value == zox_town_type_wall_edge || town_value == zox_town_type_wall_spike) {
                    byte is_spike = town_value == zox_town_type_wall_spike;
                    byte is_edge = town_value == zox_town_type_wall_edge;
                    for (byte h = 1; h <= wall_height + is_edge + is_spike * 2; h++) {
                        int global_y = height + h;
                        if (global_y >= chunk_block_position.y) {
                            position.y = (global_y - chunk_block_position.y) / hmultiplier;
                            if (position.y >= 0 && position.y < length) {
                                set_clean_VoxelNode(
                                    voctree,
                                    build_depth,
                                    position,
                                    bricks_id);
                            }
                        }
                    }
                }
                // NOTE: Town Gate! has a gap!
                else if (town_value == zox_town_type_gate) {
                    for (int h = gate_height; h <= wall_height; h++) {
                        int global_y = height + h;
                        if (global_y >= chunk_block_position.y) {
                            position.y = (global_y - chunk_block_position.y) / hmultiplier;
                            if (position.y >= 0 && position.y < length) {
                                set_clean_VoxelNode(
                                    voctree,
                                    build_depth,
                                    position,
                                    bricks_id);
                            }
                        }
                    }
                }
                // Home Walls
                else if (town_value == zox_town_type_home_wall) {
                    for (int h = 2; h < home_height; h++) {
                        int global_y = height + h;
                        if (global_y >= chunk_block_position.y) {
                            position.y = (global_y - chunk_block_position.y) / hmultiplier;
                            if (position.y >= 0 && position.y < length) {
                                set_clean_VoxelNode(
                                    voctree,
                                    build_depth,
                                    position,
                                    home_wall_id);
                            }
                        }
                    }
                }
                // Home Door Wall Overhang
                else if (town_value == zox_town_type_home_door) {
                    for (int h = home_height - 1; h < home_height; h++) {
                        int global_y = height + h;
                        if (global_y >= chunk_block_position.y) {
                            position.y = (global_y - chunk_block_position.y) / hmultiplier;
                            if (position.y >= 0 && position.y < length) {
                                set_clean_VoxelNode(
                                    voctree,
                                    build_depth,
                                    position,
                                    home_wall_id);
                            }
                        }
                    }
                }
                // Roofs: in wood, flat or peaked by its home (home_roof_rise), gables filled, glow stone set into it
                if (town_value == zox_town_type_home || town_value == zox_town_type_home_door || town_value == zox_town_type_home_wall) {
                    const int2 g = {
                        chunk_block_position.x + position.x * hmultiplier,
                        chunk_block_position.z + position.z * hmultiplier
                    };
                    const TownHome* home = town_home_at(homes, homes_count, g);
                    const int rise = home ? home_roof_rise(home, g) : 0;
                    const int roof_y = height + home_height + rise;
                    if (home && home_is_gable(home, g)) {
                        for (int y = height + home_height; y < roof_y; y++) {
                            town_set_block(voctree, build_depth, &position, y, chunk_block_position.y, hmultiplier, length, home_wall_id);
                        }
                    }
                    // the roof, its middle a glow stone set into it (lights the room under and shows outside)
                    const byte is_glow = home && glow_id && int2_equals(g, home->center);
                    if (town_set_block(voctree, build_depth, &position, roof_y, chunk_block_position.y, hmultiplier, length, is_glow ? glow_id : wood_id) &&
                        is_glow && glows < town_homes_max) {
                        glow_positions[glows++] = position;
                    }
                }
                // Floors
                if (town_value == zox_town_type_home || town_value == zox_town_type_home_door || town_value == zox_town_type_home_wall) {
                    int global_y = height + 1;
                    if (global_y >= chunk_block_position.y) {
                        position.y = (global_y - chunk_block_position.y) / hmultiplier;
                        if (position.y >= 0 && position.y < length) {
                            set_clean_VoxelNode(
                                voctree,
                                build_depth,
                                position,
                                home_floor_id);
                        }
                    }
                }
            }
        }
        zox_octree_lock_end(write_lock);
        // glow stones light the air on their sides (solid, the flood skips their own voxel), on the light queue
        if (glows && glow_light && zox_has(e, LightQueue)) {
            LightQueue* light_queue = zox_getm(e, LightQueue);
            spin_lock(&light_queue->lock);
            for (byte g = 0; g < glows; g++) {
                for (byte d = 0; d < 6; d++) {
                    const sbyte3 o = neighbor_offsets[d];
                    const int3 p = { glow_positions[g].x + o.x, glow_positions[g].y + o.y, glow_positions[g].z + o.z };
                    if (p.x < 0 || p.y < 0 || p.z < 0 || p.x >= length || p.y >= length || p.z >= length) {
                        continue;
                    }
                    a_LightQueue(light_queue, (LightUpdate) {
                        .pos = int3_to_byte3(p), .depth = build_depth, .distance = light_propogation_distance,
                        .light = glow_light > light_air_decay ? glow_light - light_air_decay : darklight });
                }
            }
            spin_unlock(&light_queue->lock);
        }
        generate->value = zox_generate_terrain_sunlight;
    }
    zox_sys_on_end();
} zoxd_system(town_walls_system);
