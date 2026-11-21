// tst: town_furniture, homes have a door and a bed, towns street lamps, regions highways (towns/sys/walls.c)
//  - each home in the player's region drawn at full detail: a door in its doorway (its base, a filler over it whose
//    block data leads back down) and a bed in its back corner (its base, a filler beside it)
//  - each street lamp column drawn at full detail: the lamp on the ground, two fillers over it leading back down
//  - run: bash bsh/tst.sh town_furniture

// a voxel's block data in a loaded chunk at full detail, -1 if it isn't loaded or not at full detail
static inline int zox_tst_furniture_data(ecs* world, entity terrain, int3 block, int* voxel) {
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    const int3 chunk_position = block_position_to_chunk_position(block, depth);
    const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, chunk_position);
    if (!zox_valid(chunk) || !zox_has(chunk, BlockDatas) || zox_getv(chunk, NodeDepth) != depth ||
        (zox_has(chunk, GenerateChunk) && zox_getv(chunk, GenerateChunk))) {
        return -1;
    }
    const byte3 local = { block.x - chunk_position.x * length, block.y - chunk_position.y * length, block.z - chunk_position.z * length };
    spinlock* lock = zox_octree_lock(world, chunk);
    zox_octree_lock_begin(lock);
    *voxel = getv_VoxelNode(zox_get(chunk, VoxelNode), depth, local);
    const int data = get_block_data(zox_get(chunk, BlockDatas), local);
    zox_octree_lock_end(lock);
    return data;
}

// a sized block's base and filler at offset from it: 1 if right, 0 if wrong, -1 if not drawn at full detail
static inline int zox_tst_furniture_at(ecs* world, entity terrain, int3 base, int3 filler_offset, int base_id, int filler_id) {
    int voxel = 0, filler_voxel = 0;
    const int base_data = zox_tst_furniture_data(world, terrain, base, &voxel);
    const int3 filler = int3_add(base, filler_offset);
    const int filler_data = zox_tst_furniture_data(world, terrain, filler, &filler_voxel);
    if (base_data < 0 || filler_data < 0) {
        return -1;
    }
    return voxel == base_id && base_data && filler_voxel == filler_id && filler_data &&
        int3_equals(int3_add(filler, block_offset_decode(filler_data)), base);
}

void zox_tst_town_furniture_check(ecs* world) {
    const entity character = zox_tst_player_character(world);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    const int door_id = zox_tst_block_id(world, realm, zox_id(BlockDoor));
    const int bed_id = zox_tst_block_id(world, realm, zox_id(BlockBed));
    const int lamp_id = zox_tst_block_id(world, realm, zox_id(BlockLamp));
    const int filler_id = zox_tst_block_id(world, realm, zox_id(BlockFiller));
    zox_tst_check(door_id && bed_id && lamp_id && filler_id, "the realm lacks a door [%i], bed [%i], lamp [%i] or filler [%i]",
        door_id, bed_id, lamp_id, filler_id);
    if (!door_id || !bed_id || !lamp_id || !filler_id) {
        return;
    }
    const entity region = zox_tst_player_region(world);
    entity towns[zox_children_capacity];
    const uint towns_count = zox_valid(region) ? zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town)) : 0;
    int homes = 0, doors = 0, beds = 0;
    for (uint t = 0; t < towns_count; t++) {
        entity children[zox_children_capacity];
        const uint count = zox_get_children_by_id(world, towns[t], children, zox_children_capacity, zox_id(Home));
        for (uint k = 0; k < count; k++) {
            const TownHome h = town_home(world, children[k]);
            const int2 half = home_half_local(&h);
            const int2 door = int2_add(h.center, build_turn(h.facing, 0, half.y));
            const int door_ok = zox_tst_furniture_at(world, terrain, (int3) { door.x, h.floor + 2, door.y }, (int3) { 0, 1, 0 }, door_id, filler_id);
            const int2 bed = int2_add(h.center, build_turn(h.facing, -half.x + 1, -half.y + 1));
            const int2 bed_end = build_turn(h.facing, 1, 0);
            const int bed_ok = zox_tst_furniture_at(world, terrain, (int3) { bed.x, h.floor + 2, bed.y }, (int3) { bed_end.x, 0, bed_end.y }, bed_id, filler_id);
            if (door_ok < 0 || bed_ok < 0) {
                continue;
            }
            homes++;
            doors += door_ok;
            beds += bed_ok;
        }
    }
    // street lamps and highways in its drawn tunks at full detail
    int lamps = 0, lamps_ok = 0, highway_columns = 0;
    iter it2 = zox_children(world, local_terrain);
    while (zox_children_next(it2)) {
        for (int i = 0; i < it2.count; i++) {
            const entity tunk = it2.entities[i];
            RegionTunk rt;
            if (!zox_has(tunk, TownMap) || !zox_has(tunk, TunkPosition) || zox_getv(tunk, GenerateTunk) ||
                !region_tunk_begin(world, tunk, zox_getv(tunk, TunkLod), zox_getv(tunk, TunkPosition), &rt) ||
                rt.depth_difference != 1 || rt.region != region) {
                continue;
            }
            const TownMap* town_map = zox_get(tunk, TownMap);
            const HeightMap* height_map = zox_get(tunk, HeightMap);
            if (town_map->length != rt.length * rt.length || height_map->length != town_map->length) {
                continue;
            }
            int2 position;
            for (position.x = 0; position.x < rt.length; position.x++) {
                for (position.y = 0; position.y < rt.length; position.y++) {
                    const int index = int2_array_index(position, rt.map_size);
                    const byte value = town_map->value[index];
                    highway_columns += value == zox_town_type_highway;
                    if (value != zox_town_type_lamp) {
                        continue;
                    }
                    const int2 g = region_tunk_global(&rt, position);
                    const int3 base = { g.x, height_map->value[index] + 1, g.y };
                    const int low = zox_tst_furniture_at(world, terrain, base, (int3) { 0, 1, 0 }, lamp_id, filler_id);
                    const int high = zox_tst_furniture_at(world, terrain, base, (int3) { 0, 2, 0 }, lamp_id, filler_id);
                    if (low < 0 || high < 0) {
                        continue;
                    }
                    lamps++;
                    lamps_ok += low && high;
                }
            }
        }
    }
    zox_log("[tst] [metric] town_furniture towns [%i] homes [%i] doors [%i] beds [%i] lamps [%i] ok [%i] highway columns [%i]",
        towns_count, homes, doors, beds, lamps, lamps_ok, highway_columns);
    zox_tst_check(homes > 0, "no homes drawn at full detail near the player");
    zox_tst_check(doors == homes, "[%i] of [%i] homes have no door in their doorway", homes - doors, homes);
    zox_tst_check(beds == homes, "[%i] of [%i] homes have no bed", homes - beds, homes);
    zox_tst_check(lamps > 0, "no street lamps drawn at full detail");
    zox_tst_check(lamps_ok == lamps, "[%i] of [%i] street lamps aren't a lamp with two fillers", lamps - lamps_ok, lamps);
}

// tst: lamp_light, a street lamp placed lights from its head (its top filler), removed it goes dark (lights3/sys/editing.c)
//  - two sealed shafts in solid ground under the player, a lamp at each one's bottom: one in a chunk, one whose head
//    is in the chunk over it (its base at the chunk's top but one)
//  - lit: the air over each head a step from its light (lit from its base, it was three steps down there);
//    removed: dark again
//  - run: bash bsh/tst.sh lamp_light
#define zox_tst_ll_lamps 2
byte zox_tst_ll_stage = 0;
double zox_tst_ll_time = 0;
entity zox_tst_ll_terrain = 0;
int3 zox_tst_ll_bases[zox_tst_ll_lamps];
int zox_tst_ll_lit[zox_tst_ll_lamps], zox_tst_ll_dark[zox_tst_ll_lamps];
int zox_tst_ll_emits = 0;

// a world block's light, -1 if its chunk isn't loaded
static inline int zox_tst_ll_light(ecs* world, entity terrain, int3 block) {
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    const int3 c = block_position_to_chunk_position(block, depth);
    const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, c);
    if (!zox_valid(chunk) || !zox_has(chunk, LightNode)) {
        return -1;
    }
    return getv_LightNode(zox_get(chunk, LightNode), depth, (byte3) { block.x - c.x * length, block.y - c.y * length, block.z - c.z * length });
}

static inline byte zox_tst_ll_set(ecs* world, entity terrain, int3 block, byte value, byte data) {
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    const int3 c = block_position_to_chunk_position(block, depth);
    const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, c);
    if (!zox_valid(chunk) || !zox_has(chunk, VoxelNodeQueue)) {
        return 0;
    }
    a_VoxelNodeQueue(zox_getm(chunk, VoxelNodeQueue), (VoxelNodeUpdate) { .value = value, .data = data,
        .position = (byte3) { block.x - c.x * length, block.y - c.y * length, block.z - c.z * length } });
    return 1;
}

// a lamp and its fillers at base (lamp 0 removes them)
static inline void zox_tst_ll_place(ecs* world, entity terrain, int3 base, byte lamp, byte filler) {
    for (int h = 0; h < 3; h++) {
        zox_tst_ll_set(world, terrain, (int3) { base.x, base.y + h, base.z }, lamp ? (h ? filler : lamp) : 0,
            lamp ? (h ? block_offset_encode((int3) { 0, -h, 0 }) : 1) : 0);
    }
}

byte zox_tst_lamp_light_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    const byte lamp = zox_tst_block_id(world, realm, zox_id(BlockLamp));
    const byte filler = zox_tst_block_id(world, realm, zox_id(BlockFiller));
    const double t = zox_current_time - zox_tst_ll_time;
    zox_tst_progress(zox_tst_ll_stage);
    if (zox_tst_ll_stage == 0) {
        zox_tst_waiting = "solid ground under the player";
        const int length = octree_size(zox_getv(terrain, NodeDepth));
        int3 c = real_position_to_chunk_position(zox_getv(character, Position3D), length, zox_getv(terrain, BlockScale));
        c.y -= 3;
        // each shaft: its lamp and a block of air over its head, sealed in solid ground a block round
        zox_tst_ll_bases[0] = (int3) { c.x * length + 4, c.y * length + 4, c.z * length + 8 };
        zox_tst_ll_bases[1] = (int3) { c.x * length + 11, c.y * length + length - 2, c.z * length + 8 };
        for (int l = 0; l < zox_tst_ll_lamps; l++) {
            const int3 b = zox_tst_ll_bases[l];
            for (int x = -1; x <= 1; x++) for (int y = -1; y <= 4; y++) for (int z = -1; z <= 1; z++) {
                if (terrain_voxel_at(world, terrain, (int3) { b.x + x, b.y + y, b.z + z }) <= 0) {
                    zox_tst_check(0, "the ground under the player isn't solid round lamp [%i]", l);
                    return 1;
                }
            }
        }
        zox_tst_check(lamp && filler, "the realm has no lamp [%i] or filler [%i]", lamp, filler);
        if (!lamp || !filler) {
            return 1;
        }
        for (int l = 0; l < zox_tst_ll_lamps; l++) {
            for (int y = 0; y <= 3; y++) {
                zox_tst_ll_set(world, terrain, (int3) { zox_tst_ll_bases[l].x, zox_tst_ll_bases[l].y + y, zox_tst_ll_bases[l].z }, 0, 0);
            }
        }
        zox_tst_ll_terrain = terrain;
        zox_tst_ll_emits = zox_getv(zox_get_child_by_id(world, realm, zox_id(BlockLamp)), BlockLight);
        zox_tst_ll_stage = 1;
        zox_tst_ll_time = zox_current_time;
        return 0;
    }
    if (zox_tst_ll_stage == 1) {
        zox_tst_waiting = "the shafts to settle dark";
        if (t < 1) {
            return 0;
        }
        for (int l = 0; l < zox_tst_ll_lamps; l++) {
            zox_tst_ll_place(world, terrain, zox_tst_ll_bases[l], lamp, filler);
        }
        zox_tst_ll_stage = 2;
        zox_tst_ll_time = zox_current_time;
        return 0;
    }
    if (zox_tst_ll_stage == 2) {
        zox_tst_waiting = "the lamps to light";
        if (t < 1.5) {
            return 0;
        }
        for (int l = 0; l < zox_tst_ll_lamps; l++) {
            const int3 b = zox_tst_ll_bases[l];
            zox_tst_ll_lit[l] = zox_tst_ll_light(world, terrain, (int3) { b.x, b.y + 3, b.z });
            zox_tst_ll_place(world, terrain, b, 0, 0);
        }
        zox_tst_ll_stage = 3;
        zox_tst_ll_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the lamps' light to go";
    if (t < 1.5) {
        return 0;
    }
    for (int l = 0; l < zox_tst_ll_lamps; l++) {
        const int3 b = zox_tst_ll_bases[l];
        zox_tst_ll_dark[l] = zox_tst_ll_light(world, terrain, (int3) { b.x, b.y + 3, b.z });
    }
    zox_tst_ll_stage = 4;
    return 1;
}

void zox_tst_lamp_light_check(ecs* world) {
    (void) world;
    if (zox_tst_ll_stage < 4) {
        return;
    }
    const int head = zox_tst_ll_emits - light_air_decay;
    zox_log("[tst] [metric] lamp_light over the heads: lit [%i %i] (from the head [%i]) dark [%i %i]",
        zox_tst_ll_lit[0], zox_tst_ll_lit[1], head, zox_tst_ll_dark[0], zox_tst_ll_dark[1]);
    zox_tst_check(zox_tst_ll_lit[0] >= head, "over the lamp's head is [%i], not [%i]: not lit from its head", zox_tst_ll_lit[0], head);
    zox_tst_check(zox_tst_ll_lit[1] >= head, "over the lamp's head in the chunk over its base is [%i], not [%i]", zox_tst_ll_lit[1], head);
    zox_tst_check(zox_tst_ll_dark[0] <= darklight + 4 && zox_tst_ll_dark[1] <= darklight + 4,
        "removed, over the lamps' heads is still lit [%i %i]", zox_tst_ll_dark[0], zox_tst_ll_dark[1]);
}

// tst: highways, the highways round the player (fun/highways.c) reach its region's towns and go round them
//  - worked out as tunks do over a box round the player (highways_in_box), its regions' towns placed
//  - each town in the player's region has a highway from its gate
//  - along each segment a block at a time: none inside a town's walls (their curve) or a mountain (the region's
//    circles; a town's link may, if no other line is clear)
//  - its drawn tunks at full detail mark each column on a highway's middle line (worked out again per tunk)
//  - run: bash bsh/tst.sh highways
byte zox_tst_highways_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_waiting = "the player's region's towns";
    const entity region = zox_tst_player_region(world);
    return zox_valid(region) && !zox_getv(region, GenerateRegion);
}

void zox_tst_highways_check(ecs* world) {
    const entity region = zox_tst_player_region(world);
    const entity terrain = zox_get_parent(world, region);
    Highways h;
    highways_begin(&h, world, terrain, zox_getv(region, BlockPosition2));
    const int2 start = zox_getv(region, BlockPosition2), size = zox_getv(region, BlockSize2);
    int2 ends[1024];
    const int count = highways_in_box(&h, start, int2_add(start, int2_sub(size, int2_one)), ends, 1024);
    entity towns[zox_children_capacity];
    const uint towns_count = zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town));
    int linked = 0, through_towns = 0, through_mountains = 0;
    entity zones[zox_children_capacity];
    const uint zones_count = zox_get_children_by_id(world, region, zones, zox_children_capacity, zox_id(PlacementZone));
    for (uint t = 0; t < towns_count; t++) {
        const int2 gate_out = int2_add(town_gate_point(world, towns[t]), build_turn(zox_getv(towns[t], TownGateSide), 0, 1));
        byte found = 0;
        for (int i = 0; i < count && !found; i++) {
            found = int2_equals(ends[i], gate_out);
        }
        linked += found;
    }
    for (int i = 0; i + 1 < count; i += 2) {
        const int2 a = ends[i], b = ends[i + 1];
        const int steps = int_max(abs(b.x - a.x), abs(b.y - a.y));
        // a town's link may cross a mountain if no other line is clear (highway_town_link): mountains checked on the
        //  lattice's links
        byte town_link = 0;
        for (byte r = 0; r < h.regions_count && !town_link; r++) {
            for (byte k = 0; k < h.regions[r].count && !town_link; k++) {
                town_link = int2_equals(a, h.regions[r].towns[k].out) || int2_equals(b, h.regions[r].towns[k].out);
            }
        }
        for (int k = 0; k <= steps; k++) {
            const float f = steps ? (float) k / steps : 0;
            const int2 g = { (int) roundf(a.x + (b.x - a.x) * f), (int) roundf(a.y + (b.y - a.y) * f) };
            for (uint t = 0; t < towns_count; t++) {
                const TownShape shape = town_shape_of(world, towns[t]);
                through_towns += town_shape_depth(&shape, g) >= 0;
            }
            for (uint z = 0; z < zones_count && !town_link; z++) {
                if (!zox_has(zones[z], Radius)) {
                    continue;
                }
                const int2 c = zox_getv(zones[z], BlockPosition2);
                const int r = zox_getv(zones[z], Radius);
                through_mountains += (g.x - c.x) * (g.x - c.x) + (g.y - c.y) * (g.y - c.y) < r * r;
            }
        }
    }
    // drawn tunks at full detail: each column on a highway's middle line is marked (highway, or a town's)
    int columns = 0, missing = 0;
    iter it2 = zox_children(world, local_terrain);
    while (zox_children_next(it2)) {
        for (int i = 0; i < it2.count; i++) {
            const entity tunk = it2.entities[i];
            RegionTunk rt;
            if (!zox_has(tunk, TownMap) || !zox_has(tunk, TunkPosition) || zox_getv(tunk, GenerateTunk) ||
                !region_tunk_begin(world, tunk, zox_getv(tunk, TunkLod), zox_getv(tunk, TunkPosition), &rt) ||
                rt.depth_difference != 1 || zox_get(tunk, TownMap)->length != rt.length * rt.length) {
                continue;
            }
            const TownMap* map = zox_get(tunk, TownMap);
            Highways th;
            highways_begin(&th, world, terrain, rt.global_start);
            int2 tunk_ends[64];
            const int tunk_count = highways_in_box(&th, rt.global_start, int2_add(rt.global_start, int2_single(rt.length - 1)), tunk_ends, 64);
            for (int e2 = 0; e2 + 1 < tunk_count; e2 += 2) {
                const int2 a = tunk_ends[e2], b = tunk_ends[e2 + 1];
                const int steps = int_max(abs(b.x - a.x), abs(b.y - a.y));
                for (int k = 0; k <= steps; k++) {
                    const float f = steps ? (float) k / steps : 0;
                    const int2 p = int2_sub((int2) { (int) roundf(a.x + (b.x - a.x) * f), (int) roundf(a.y + (b.y - a.y) * f) }, rt.global_start);
                    if (p.x < 0 || p.y < 0 || p.x >= rt.length || p.y >= rt.length) {
                        continue;
                    }
                    const byte v = map->value[int2_array_index(p, rt.map_size)];
                    columns += v == zox_town_type_highway;
                    missing += v == zox_town_type_none;
                }
            }
        }
    }
    zox_log("[tst] [metric] highways towns [%i] linked [%i] segments [%i] blocks through towns [%i] mountains [%i] columns drawn [%i] missing [%i]",
        towns_count, linked, count / 2, through_towns, through_mountains, columns, missing);
    zox_tst_check(count >= 2, "no highways in the player's region");
    zox_tst_check(linked == (int) towns_count, "[%i] of [%i] towns have no highway from their gate", towns_count - linked, towns_count);
    zox_tst_check(!through_towns, "[%i] highway blocks inside towns", through_towns);
    zox_tst_check(!through_mountains, "[%i] highway blocks inside mountains", through_mountains);
    zox_tst_check(!missing, "[%i] columns on highways' middle lines aren't drawn", missing);
}
