// tst: town_maps, towns draw into every tunk they cross, walls join up across tunk edges
//  - each generated tunk at full detail: for every town in its region, every column of the tunk inside it
//    (not region_tunk_box, which the maps use to draw)
//  - its outer ring is wall (a wall, edge, spike or the gate), every column inside the town is marked
//  - counts towns crossing tunks, so walls drawn by more than one tunk are checked
//  - run: bash bsh/tst.sh town_maps
int zox_tst_town_maps_frames = 0;

byte zox_tst_town_maps_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    // a few frames for nearby tunks' towns to draw
    zox_tst_waiting = "towns to draw into tunks";
    return ++zox_tst_town_maps_frames > 30;
}

static inline byte zox_tst_is_town_wall(byte value) {
    return value == zox_town_type_wall || value == zox_town_type_wall_edge ||
        value == zox_town_type_wall_spike || value == zox_town_type_gate;
}

void zox_tst_town_maps_check(ecs* world) {
    int tunks = 0, columns = 0, ring_wrong = 0, inside_unmarked = 0;
    // towns seen, and how many tunks each was drawn in
    entity towns_seen[256];
    int towns_tunks[256];
    int towns_count = 0;
    iter it2 = zox_children(world, local_terrain);
    while (zox_children_next(it2)) {
        for (int i = 0; i < it2.count; i++) {
            const entity tunk = it2.entities[i];
            // drawn tunks (GenerateTunk is back to 0 once a tunk is done, its town map is sized)
            if (!zox_has(tunk, TownMap) || !zox_has(tunk, TunkPosition) || !zox_has(tunk, TunkLod) ||
                zox_getv(tunk, GenerateTunk))
            {
                continue;
            }
            RegionTunk rt;
            if (!region_tunk_begin(world, tunk, zox_getv(tunk, TunkLod), zox_getv(tunk, TunkPosition), &rt) ||
                rt.depth_difference != 1)
            {
                continue;
            }
            const TownMap* town_map = zox_get(tunk, TownMap);
            if (town_map->length != rt.length * rt.length) {
                continue;
            }
            tunks++;
            entity towns[zox_children_capacity];
            const uint towns_length = zox_get_children_by_id(world, rt.region, towns, zox_children_capacity, zox_id(Town));
            for (uint j = 0; j < towns_length; j++) {
                const int2 center = zox_getv(towns[j], BlockPosition2);
                const byte2 size = zox_getv(towns[j], TownSize);
                const int left = center.x - size.x / 2, right = center.x + size.x / 2;
                const int bottom = center.y - size.y / 2, top = center.y + size.y / 2;
                // every column of the tunk, not region_tunk_box (the maps use it, a bug there would hide here)
                int2 position;
                byte in_tunk = 0;
                for (position.x = 0; position.x < rt.length && !in_tunk; position.x++) {
                    for (position.y = 0; position.y < rt.length && !in_tunk; position.y++) {
                        const int2 g = region_tunk_global(&rt, position);
                        in_tunk = g.x >= left && g.x <= right && g.y >= bottom && g.y <= top;
                    }
                }
                if (!in_tunk) {
                    continue;
                }
                int t = 0;
                for (; t < towns_count && towns_seen[t] != towns[j]; t++) { }
                if (t == towns_count && towns_count < 256) {
                    towns_seen[towns_count] = towns[j];
                    towns_tunks[towns_count++] = 0;
                }
                if (t < 256) {
                    towns_tunks[t]++;
                }
                for (position.x = 0; position.x < rt.length; position.x++) {
                    for (position.y = 0; position.y < rt.length; position.y++) {
                        const int2 g = region_tunk_global(&rt, position);
                        if (g.x < left || g.x > right || g.y < bottom || g.y > top) {
                            continue;
                        }
                        const byte value = town_map->value[int2_array_index(position, rt.map_size)];
                        columns++;
                        if (g.x == left || g.x == right || g.y == bottom || g.y == top) {
                            ring_wrong += !zox_tst_is_town_wall(value);
                        } else {
                            inside_unmarked += value == zox_town_type_none;
                        }
                    }
                }
            }
        }
    }
    int crossing = 0;
    for (int t = 0; t < towns_count; t++) {
        crossing += towns_tunks[t] > 1;
    }
    zox_log("[tst] [metric] town_maps tunks [%i] towns [%i] crossing tunks [%i] columns checked [%i]",
        tunks, towns_count, crossing, columns);
    zox_tst_check(towns_count > 0, "no towns drawn near the player in [%i] tunks", tunks);
    zox_tst_check(crossing > 0, "no town crosses tunks, the across tunks case isn't checked");
    zox_tst_check(!ring_wrong, "[%i] columns on a town's outer ring aren't wall", ring_wrong);
    zox_tst_check(!inside_unmarked, "[%i] columns inside a town aren't marked", inside_unmarked);
}

// tst: town_maps_empty, a tunk in a region with no towns still finishes (the rare terrain loading stall)
//  - a done tunk's region loses its towns, the tunk goes back to its towns step, it ends (GenerateTunk 0)
//  - it used to stay at the towns step forever, its chunks waited on it and the player's spawn on them
//  - run: bash bsh/tst.sh town_maps_empty
entity zox_tst_empty_tunk = 0;
double zox_tst_empty_time = 0;

byte zox_tst_town_maps_empty_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    if (!zox_tst_empty_tunk) {
        zox_tst_waiting = "a finished tunk with a region";
        iter it2 = zox_children(world, local_terrain);
        while (zox_children_next(it2)) {
            for (int i = 0; i < it2.count && !zox_tst_empty_tunk; i++) {
                const entity tunk = it2.entities[i];
                RegionTunk rt;
                if (!zox_has(tunk, TownMap) || !zox_has(tunk, TunkPosition) || zox_getv(tunk, GenerateTunk) ||
                    !region_tunk_begin(world, tunk, zox_getv(tunk, TunkLod), zox_getv(tunk, TunkPosition), &rt)) {
                    continue;
                }
                entity towns[zox_children_capacity];
                const uint towns_length = zox_get_children_by_id(world, rt.region, towns, zox_children_capacity, zox_id(Town));
                for (uint j = 0; j < towns_length; j++) {
                    zox_delete(towns[j]);
                }
                zox_setv(tunk, GenerateTunk, zox_generate_tunk_towns);
                zox_tst_empty_tunk = tunk;
                zox_tst_empty_time = zox_current_time;
                zox_log("[tst] tunk [%s] back to its towns step, its region's [%i] towns deleted", zox_get_name(tunk), towns_length);
            }
        }
        return 0;
    }
    zox_tst_waiting = "the tunk to finish";
    return !zox_getv(zox_tst_empty_tunk, GenerateTunk) || zox_current_time - zox_tst_empty_time > 5;
}

void zox_tst_town_maps_empty_check(ecs* world) {
    zox_tst_check(zox_valid(zox_tst_empty_tunk), "no tunk tested");
    if (zox_valid(zox_tst_empty_tunk)) {
        const byte generate = zox_getv(zox_tst_empty_tunk, GenerateTunk);
        zox_tst_check(!generate, "the tunk in a region with no towns is stuck at step [%i]", generate);
    }
}

// tst: region_zones, a region's placement zones (regions/fun/zones.c) don't overlap
//  - the player's region: its mountains, towns and dungeons found as PlacementZone children, every rect apart from
//    every other rect and circle
//  - run: bash bsh/tst.sh region_zones
byte zox_tst_region_zones_ready(ecs* world) {
    return zox_tst_player_spawn_ready(world);
}

void zox_tst_region_zones_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const int chunk_length = octree_size(zox_getv(terrain, NodeDepth));
    const int3 chunk = real_position_to_chunk_position(zox_getv(character, Position3D), chunk_length, zox_getv(terrain, BlockScale));
    const entity tunk = int2_hashmap_get(zox_get(terrain, TunkLinks)->value, (int2) { chunk.x, chunk.z });
    const entity region = zox_valid(tunk) ? zox_get_link(world, tunk, RegionLink) : 0;
    zox_tst_check(zox_valid(region), "no region under the player");
    if (!zox_valid(region)) {
        return;
    }
    RegionZones zones;
    region_zones_find(world, region, &zones);
    zox_log("[tst] [metric] region_zones [%i] circles [%i] rects", zones.circles, zones.rects);
    zox_tst_check(zones.circles + zones.rects, "the region has no placement zones");
    int overlaps = 0;
    for (int a = 0; a < zones.rects; a++) {
        const int2 p = zones.rect_positions[a];
        const byte2 s = zones.rect_sizes[a];
        for (int b = 0; b < a; b++) {
            const int2 q = zones.rect_positions[b];
            const byte2 t = zones.rect_sizes[b];
            overlaps += abs(p.x - q.x) < (s.x + t.x) / 2 && abs(p.y - q.y) < (s.y + t.y) / 2;
        }
        overlaps += is_overlap_points(zones.circle_positions, zones.circle_radii, zones.circles, p, s);
    }
    zox_tst_check(!overlaps, "[%i] placement zones overlap", overlaps);
}

// tst: home_roofs, roof shapes (build_home, towns/fun/home.c): flat is flat, peaked rises a block a column in from its
//  long sides to a ridge, symmetric, gables at its ends, a turned home's ridge turns with it
//  - run: bash bsh/tst.sh home_roofs
// the highest box of a block over a column, -1 if none
static inline int zox_tst_build_top(const Building* b, int2 g, byte block) {
    int top = -1;
    for (byte i = 0; i < b->count; i++) {
        const BuildBox* box = &b->boxes[i];
        if (box->block == block && g.x >= box->min.x && g.x <= box->max.x && g.y >= box->min.z && g.y <= box->max.z) {
            top = int_max(top, box->max.y);
        }
    }
    return top;
}

void zox_tst_home_roofs_check(ecs* world) {
    (void) world;
    const HomeBlocks blocks = { .wall = 1, .floor = 2, .roof = 3, .glow = 0 };
    const TownHome flat = { { 100, 100 }, { 5, 3 }, 0, 10, 0, 6 };
    const TownHome peaked = { { 100, 100 }, { 5, 3 }, 1, 10, 0, 6 };   // longer along x: its ridge runs along x
    const TownHome turned = { { 100, 100 }, { 3, 5 }, 1, 10, 1, 6 };   // facing +x, longer along z: its ridge along z
    Building b;
    int3 glow;
    const int roof = 10 + 6;
    build_home(&b, &flat, &blocks, &glow);
    int flat_rises = 0;
    for (int x = 95; x <= 105; x++) for (int y = 97; y <= 103; y++) {
        flat_rises += zox_tst_build_top(&b, (int2) { x, y }, 3) != roof;
    }
    zox_tst_check(!flat_rises, "a flat roof isn't flat at [%i] columns", flat_rises);
    build_home(&b, &peaked, &blocks, &glow);
    #define zox_tst_rise(g) (zox_tst_build_top(&b, g, 3) - roof)
    zox_tst_check(zox_tst_rise(((int2) { 100, 97 })) == 0, "a peaked roof doesn't start at its wall");
    zox_tst_check(zox_tst_rise(((int2) { 100, 100 })) == 3, "its ridge isn't 3 up, it's [%i]", zox_tst_rise(((int2) { 100, 100 })));
    int asymmetric = 0, steps = 0;
    for (int y = 97; y <= 100; y++) {
        asymmetric += zox_tst_rise(((int2) { 102, y })) != zox_tst_rise(((int2) { 102, 200 - y }));
        steps += y > 97 && zox_tst_rise(((int2) { 102, y })) != zox_tst_rise(((int2) { 102, y - 1 })) + 1;
    }
    zox_tst_check(!asymmetric && !steps, "the roof isn't a symmetric slope, [%i] uneven, [%i] steps off", asymmetric, steps);
    // gables: its ends walled up to the roof, a side isn't
    zox_tst_check(zox_tst_build_top(&b, (int2) { 95, 100 }, 1) == roof + 2 && zox_tst_build_top(&b, (int2) { 105, 100 }, 1) == roof + 2,
        "its ends aren't gables, walled to [%i]", zox_tst_build_top(&b, (int2) { 95, 100 }, 1));
    zox_tst_check(zox_tst_build_top(&b, (int2) { 100, 97 }, 1) == roof - 1, "a side is walled over its wall height");
    zox_tst_check(int3_equals(glow, (int3) { 100, roof + 3, 100 }), "its glow stone isn't at its ridge's middle");
    // turned: the same roof, its ridge along z
    build_home(&b, &turned, &blocks, &glow);
    zox_tst_check(zox_tst_rise(((int2) { 97, 100 })) == 0 && zox_tst_rise(((int2) { 100, 100 })) == 3 && zox_tst_rise(((int2) { 100, 103 })) == 3,
        "a turned home's ridge doesn't run along z");
    #undef zox_tst_rise
}

// the region of the player's column
static inline entity zox_tst_player_region(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const int chunk_length = octree_size(zox_getv(terrain, NodeDepth));
    const int3 chunk = real_position_to_chunk_position(zox_getv(character, Position3D), chunk_length, zox_getv(terrain, BlockScale));
    const entity tunk = int2_hashmap_get(zox_get(terrain, TunkLinks)->value, (int2) { chunk.x, chunk.z });
    return zox_valid(tunk) ? zox_get_link(world, tunk, RegionLink) : 0;
}

// tst: town_homes, homes are placed inside their towns, apart from each other, a few a town, styles mixed
//  - run: bash bsh/tst.sh town_homes
byte zox_tst_town_homes_ready(ecs* world) {
    return zox_tst_player_spawn_ready(world);
}

void zox_tst_town_homes_check(ecs* world) {
    const entity region = zox_tst_player_region(world);
    zox_tst_check(zox_valid(region), "no region under the player");
    entity towns[zox_children_capacity];
    const uint towns_count = zox_valid(region) ? zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town)) : 0;
    int homes_total = 0, outside = 0, overlaps = 0, peaked = 0;
    for (uint t = 0; t < towns_count; t++) {
        const int2 tc = zox_getv(towns[t], BlockPosition2);
        const byte2 ts = zox_getv(towns[t], TownSize);
        entity homes[zox_children_capacity];
        const uint count = zox_get_children_by_id(world, towns[t], homes, zox_children_capacity, zox_id(Home));
        for (uint a = 0; a < count; a++) {
            const int2 c = zox_getv(homes[a], BlockPosition2);
            const byte2 s = zox_getv(homes[a], HomeSize);
            outside += c.x - s.x / 2 < tc.x - ts.x / 2 || c.x + s.x / 2 > tc.x + ts.x / 2 ||
                c.y - s.y / 2 < tc.y - ts.y / 2 || c.y + s.y / 2 > tc.y + ts.y / 2;
            peaked += zox_getv(homes[a], HomeStyle);
            for (uint b = 0; b < a; b++) {
                const int2 d = zox_getv(homes[b], BlockPosition2);
                const byte2 r = zox_getv(homes[b], HomeSize);
                overlaps += abs(c.x - d.x) <= (s.x + r.x) / 2 && abs(c.y - d.y) <= (s.y + r.y) / 2;
            }
        }
        homes_total += count;
    }
    zox_log("[tst] [metric] town_homes [%i] towns [%i] homes, [%i] peaked", towns_count, homes_total, peaked);
    zox_tst_check(homes_total, "no homes in the region's towns");
    zox_tst_check(!outside, "[%i] homes outside their towns", outside);
    zox_tst_check(!overlaps, "[%i] homes overlap", overlaps);
}

// tst: home_glow, a home's glow stone is set into its roof and lights the room under it (towns/sys/walls.c)
//  - run: bash bsh/tst.sh home_glow
double zox_tst_glow_start = 0;

byte zox_tst_home_glow_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    if (!zox_tst_glow_start) {
        zox_tst_glow_start = zox_current_time;
    }
    zox_tst_waiting = "lights to flood";
    return zox_current_time - zox_tst_glow_start > 3;
}

void zox_tst_home_glow_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    const entity glow = zox_get_child_by_id(world, realm, zox_id(BlockGlowStone));
    zox_tst_check(zox_valid(glow), "the realm has no glow stone block");
    if (!zox_valid(glow)) {
        return;
    }
    const int glow_id = zox_getv(glow, BlockIndex);
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    const ChunkLinks* chunks = zox_get(terrain, ChunkLinks);
    const entity region = zox_tst_player_region(world);
    entity towns[zox_children_capacity];
    const uint towns_count = zox_valid(region) ? zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town)) : 0;
    int found = 0, lit = 0, checked = 0;
    for (uint t = 0; t < towns_count; t++) {
        entity homes[zox_children_capacity];
        const uint count = zox_get_children_by_id(world, towns[t], homes, zox_children_capacity, zox_id(Home));
        for (uint h = 0; h < count; h++) {
            const int2 c = zox_getv(homes[h], BlockPosition2);
            // up the column for the glow stone
            for (int y = -64; y < 128; y++) {
                const int3 block = { c.x, y, c.y };
                if (terrain_voxel_at(world, terrain, block) != glow_id) {
                    continue;
                }
                checked++;
                found++;
                // the room under it is lit by it (roofed, the sun doesn't reach in, only through the door)
                const int3 above = { c.x, y - 1, c.y };
                const int3 chunk_position = block_position_to_chunk_position(above, depth);
                const entity chunk = int3_hashmap_get(chunks->value, chunk_position);
                if (zox_valid(chunk) && zox_has(chunk, LightNode)) {
                    const byte3 local = { above.x - chunk_position.x * length, above.y - chunk_position.y * length, above.z - chunk_position.z * length };
                    const byte light = getv_LightNode(zox_get(chunk, LightNode), depth, local);
                    lit += light > darklight + 50;
                }
                break;
            }
        }
    }
    zox_log("[tst] [metric] home_glow [%i] glow stones found, [%i] lit around", found, lit);
    zox_tst_check(found, "no glow stones on the homes");
    zox_tst_check(lit == found, "[%i] of [%i] glow stones don't light the room under them", found - lit, found);
}

// tst: glow_light, a glow stone lights a dark tunnel, fading with distance, and removing it darkens it again
//  - digs a sealed tunnel in a solid chunk under the player (local x 4 to 11), places a glow stone at its end
//  - lit: the air beside it bright, fading along the tunnel; removed: the tunnel back to darklight
//    (removing an emitter left its light, lights3/sys/editing.c darkens from it now)
//  - run: bash bsh/tst.sh glow_light
byte zox_tst_glow_stage = 0;
double zox_tst_glow_time = 0;
entity zox_tst_glow_chunk = 0;
byte zox_tst_glow_lit[8];
byte zox_tst_glow_dark[8];

static inline void zox_tst_glow_set(ecs* world, entity chunk, byte3 position, byte value) {
    VoxelNodeQueue* queue = zox_getm(chunk, VoxelNodeQueue);
    a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = value, .position = position });
}

static inline void zox_tst_glow_read(ecs* world, entity chunk, byte* lights) {
    const byte depth = zox_getv(chunk, NodeDepth);
    for (byte i = 0; i < 8; i++) {
        lights[i] = getv_LightNode(zox_get(chunk, LightNode), depth, (byte3) { 4 + i, 8, 8 });
    }
}

byte zox_tst_glow_light_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const double t = zox_current_time - zox_tst_glow_time;
    zox_tst_progress(zox_tst_glow_stage);
    if (zox_tst_glow_stage == 0) {
        zox_tst_waiting = "a solid chunk under the player";
        const byte depth = zox_getv(terrain, NodeDepth);
        const int length = octree_size(depth);
        int3 chunk_position = real_position_to_chunk_position(zox_getv(character, Position3D), length, zox_getv(terrain, BlockScale));
        chunk_position.y -= 2;
        const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, chunk_position);
        if (!zox_valid(chunk) || !zox_has(chunk, VoxelNodeQueue)) {
            return 0;
        }
        // the tunnel and its walls all solid, so it's sealed and dark
        int open = 0;
        for (int x = 3; x <= 12; x++) for (int y = 7; y <= 9; y++) for (int z = 7; z <= 9; z++) {
            open += !getv_VoxelNode(zox_get(chunk, VoxelNode), depth, (byte3) { x, y, z });
        }
        zox_tst_check(!open, "the chunk under the player isn't solid where the tunnel goes ([%i] open)", open);
        if (open) {
            return 1;
        }
        for (byte x = 4; x <= 11; x++) {
            zox_tst_glow_set(world, chunk, (byte3) { x, 8, 8 }, 0);
        }
        zox_tst_glow_chunk = chunk;
        zox_tst_glow_stage = 1;
        zox_tst_glow_time = zox_current_time;
        return 0;
    }
    const entity chunk = zox_tst_glow_chunk;
    if (zox_tst_glow_stage == 1) {
        zox_tst_waiting = "the tunnel to settle dark";
        if (t < 1) {
            return 0;
        }
        const entity glow = zox_get_child_by_id(world, zox_get_link(world, character, RealmLink), zox_id(BlockGlowStone));
        zox_tst_glow_set(world, chunk, (byte3) { 4, 8, 8 }, zox_getv(glow, BlockIndex));
        zox_tst_glow_stage = 2;
        zox_tst_glow_time = zox_current_time;
        return 0;
    }
    if (zox_tst_glow_stage == 2) {
        zox_tst_waiting = "the glow stone to light the tunnel";
        if (t < 1.5) {
            return 0;
        }
        zox_tst_glow_read(world, chunk, zox_tst_glow_lit);
        zox_tst_glow_set(world, chunk, (byte3) { 4, 8, 8 }, 0);
        zox_tst_glow_stage = 3;
        zox_tst_glow_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the tunnel to go dark";
    if (t < 1.5) {
        return 0;
    }
    zox_tst_glow_read(world, chunk, zox_tst_glow_dark);
    return 1;
}

void zox_tst_glow_light_check(ecs* world) {
    (void) world;
    if (zox_tst_glow_stage < 3) {
        return;
    }
    zox_log("[tst] lit [%i %i %i %i %i %i %i] dark [%i %i %i %i %i %i %i]",
        zox_tst_glow_lit[1], zox_tst_glow_lit[2], zox_tst_glow_lit[3], zox_tst_glow_lit[4], zox_tst_glow_lit[5], zox_tst_glow_lit[6], zox_tst_glow_lit[7],
        zox_tst_glow_dark[1], zox_tst_glow_dark[2], zox_tst_glow_dark[3], zox_tst_glow_dark[4], zox_tst_glow_dark[5], zox_tst_glow_dark[6], zox_tst_glow_dark[7]);
    // 0 was the glow stone, 1 beside it
    zox_tst_check(zox_tst_glow_lit[1] > darklight + 100, "beside the glow stone is [%i]", zox_tst_glow_lit[1]);
    int rising = 0;
    for (int i = 2; i < 8; i++) {
        rising += zox_tst_glow_lit[i] > zox_tst_glow_lit[i - 1];
    }
    zox_tst_check(!rising, "the light doesn't fade along the tunnel ([%i] rises)", rising);
    int lit_after = 0;
    for (int i = 0; i < 8; i++) {
        lit_after += zox_tst_glow_dark[i] > darklight + 4;
    }
    zox_tst_check(!lit_after, "[%i] tunnel voxels still lit after the glow stone was removed", lit_after);
}

// tst: block_table, a realm saves its block table once generated (chunks3/io/sys/block_table.c): blocks.txt in its
//  folder, a line a block, in BlockLinks order
//  - run: bash bsh/tst.sh block_table
void zox_tst_block_table_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    zox_tst_check(zox_has(realm, FolderPath), "the realm has no folder");
    if (!zox_has(realm, FolderPath)) {
        return;
    }
    char* path = join_path(zox_get(realm, FolderPath)->value, "blocks.txt");
    static char keys[block_table_max][block_table_key];
    const int lines = block_table_read(path, keys);
    const BlockLinks* blocks = zox_get(realm, BlockLinks);
    zox_log("[tst] [metric] block_table [%i] lines, [%i] blocks, [%s]", lines, blocks->length, path);
    zox_tst_check(lines == blocks->length, "[%s] has [%i] lines for [%i] blocks", path, lines, blocks->length);
    int wrong = 0;
    for (int i = 0; i < lines && i < blocks->length; i++) {
        char key[block_table_key];
        block_table_key_of(world, blocks->value[i], key);
        wrong += strcmp(key, keys[i]) != 0;
    }
    zox_tst_check(!wrong, "[%i] lines don't match their block", wrong);
    free(path);
}

// tst: home_foundations, homes stand level on their ground (towns/fun/home.c): the ground under them levelled to
//  their floor (the terrain at their middle), steps a block at a time from their doors
//  - each home in full detail chunks (near the player): inside, its floor block is solid and the room over it air
//  - in front of its door, the ground's top steps at most a block a column (until something built stands there)
//  - its doorway open, facing any way (HomeFacing, built in its own frame, fun/build.c)
//  - run: bash bsh/tst.sh home_foundations
void zox_tst_home_foundations_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const float3 player = float3_divide_float(zox_getv(character, Position3D), zox_getv(terrain, BlockScale));
    const entity region = zox_tst_player_region(world);
    entity towns[zox_children_capacity];
    const uint towns_count = zox_valid(region) ? zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town)) : 0;
    int homes_checked = 0, floors_wrong = 0, rooms_filled = 0, doors_shut = 0, steps_checked = 0, steps_wrong = 0;
    for (uint t = 0; t < towns_count; t++) {
        entity homes[zox_children_capacity];
        const uint count = zox_get_children_by_id(world, towns[t], homes, zox_children_capacity, zox_id(Home));
        for (uint k = 0; k < count; k++) {
            const TownHome h = town_home(world, homes[k]);
            // full detail chunks only, far ones are lods
            if (fabsf(h.center.x - player.x) > 40 || fabsf(h.center.y - player.z) > 40) {
                continue;
            }
            const int floor = h.floor;
            if (terrain_voxel_at(world, terrain, (int3) { h.center.x, floor + 1, h.center.y }) < 0) {
                continue;
            }
            homes_checked++;
            for (int x = h.center.x - h.half.x + 1; x < h.center.x + h.half.x; x++) {
                for (int z = h.center.y - h.half.y + 1; z < h.center.y + h.half.y; z++) {
                    floors_wrong += terrain_voxel_at(world, terrain, (int3) { x, floor + 1, z }) == 0;
                    rooms_filled += terrain_voxel_at(world, terrain, (int3) { x, floor + 2, z }) > 0;
                }
            }
            // its doorway open, its steps from its floor block (floor + 1) out of it, the way it faces (its own frame)
            const int2 half = home_half_local(&h);
            const int2 door = int2_add(h.center, build_turn(h.facing, 0, half.y));
            doors_shut += terrain_voxel_at(world, terrain, (int3) { door.x, floor + 2, door.y }) > 0;
            for (int lx = -home_door_width / 2; lx <= home_door_width / 2; lx++) {
                int last = floor + 1;
                for (int d = 1; d <= home_steps; d++) {
                    const int2 g = int2_add(h.center, build_turn(h.facing, lx, half.y + d));
                    const int x = g.x, z = g.y;
                    // the ground's top near its floor, stopping at something built (a wall, another home)
                    if (terrain_voxel_at(world, terrain, (int3) { x, floor + 3, z }) != 0) {
                        break;
                    }
                    // or at another home's levelled ground (its margin, a block round it)
                    byte neighbour = 0;
                    for (uint k2 = 0; k2 < count && !neighbour; k2++) {
                        const TownHome o = town_home(world, homes[k2]);
                        neighbour = k2 != k && abs(x - o.center.x) <= o.half.x + 1 && abs(z - o.center.y) <= o.half.y + 1;
                    }
                    if (neighbour) {
                        break;
                    }
                    int top = floor - home_steps - 2;
                    for (int y = floor + 2; y >= floor - home_steps - 2; y--) {
                        if (terrain_voxel_at(world, terrain, (int3) { x, y, z }) > 0) {
                            top = y;
                            break;
                        }
                    }
                    steps_checked++;
                    steps_wrong += abs(top - last) > 1;
                    last = top;
                }
            }
        }
    }
    zox_log("[tst] [metric] home_foundations [%i] homes, floors missing [%i], rooms filled [%i], doors shut [%i], steps [%i] too tall [%i]",
        homes_checked, floors_wrong, rooms_filled, doors_shut, steps_checked, steps_wrong);
    zox_tst_check(!doors_shut, "[%i] homes' doorways are walled up", doors_shut);
    zox_tst_check(homes_checked, "no homes near the player to check");
    zox_tst_check(!floors_wrong, "[%i] columns of homes have no floor (holes)", floors_wrong);
    zox_tst_check(!rooms_filled, "[%i] columns of homes' rooms are filled in", rooms_filled);
    zox_tst_check(!steps_wrong, "[%i] of [%i] steps from doors are taller than a block", steps_wrong, steps_checked);
}

// tst: town_view, a screenshot over the nearest town looking down (tst/town_view.ppm, --nohud from tst.sh), to see its
//  homes, which way they face, its roads; add --1080p to see it at the game's size
//  - run: bash bsh/tst.sh town_view
byte zox_tst_town_view_stage = 0;
double zox_tst_town_view_time = 0;

byte zox_tst_town_view_ready(ecs* world) {
    if (!zox_tst_town_view_stage) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_town_view_stage = 1;
        zox_tst_town_view_time = zox_current_time;
    }
    zox_tst_waiting = "the town view";
    zox_tst_progress(zox_tst_town_view_stage);
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const float scale = zox_getv(terrain, BlockScale);
    // over the town nearest the player, looking down
    const entity region = zox_tst_player_region(world);
    entity towns[zox_children_capacity];
    const uint towns_count = zox_valid(region) ? zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town)) : 0;
    if (!towns_count) {
        zox_tst_check(0, "no town in the player's region");
        return 1;
    }
    const float3 player = zox_getv(character, Position3D);
    int2 town = zox_getv(towns[0], BlockPosition2);
    for (uint t = 1; t < towns_count; t++) {
        const int2 c = zox_getv(towns[t], BlockPosition2);
        if (int2_distance(c, (int2) { player.x / scale, player.z / scale }) < int2_distance(town, (int2) { player.x / scale, player.z / scale })) {
            town = c;
        }
    }
    // above its homes' floors, back from its middle (the camera faces +z), the fog thinned to see across it
    entity homes[zox_children_capacity];
    const uint homes_count = zox_get_children_by_id(world, towns[0], homes, zox_children_capacity, zox_id(Home));
    const int ground = homes_count ? zox_getv(homes[0], HomeFloor) : 32;
    fog_density = 0.008f;
    zox_setv(character, Position3D, ((float3) { town.x * scale, (ground + 44) * scale, (town.y - 36) * scale }));
    zox_setv(character, Velocity3D, float3_zero);
    zox_setv(character, RenderDisabled, 1);
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    const entity head_camera = zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
    if (zox_valid(head_camera)) {
        zox_setv(head_camera, LocalRotation3D, euler_to_quaternion((float3) { -0.85f, is_camera_positive_z ? M_PI : 0, 0 }));
    }
    if (zox_tst_town_view_stage == 1) {
        if (zox_current_time - zox_tst_town_view_time < 6) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("town_view.ppm"));
        zox_tst_town_view_stage = 2;
        return 0;
    }
    return !zox_screenshot_request[0];
}

void zox_tst_town_view_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_clouds_share(zox_tst_path("town_view.ppm")) >= 0, "no screenshot saved");
}
