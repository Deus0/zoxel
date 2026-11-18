// tst: water_flow, water falls, spreads thinner and drains (terrain/sys/water_flow.c)
//  - no player: in an all air chunk (air_chunk.c), a
//      floor of bricks 13 x 13 (local y 3), then water (a source) 2 blocks over its middle
//  - it falls to the floor (local y 4, its fall level 7) and spreads over it a level less a block: the blocks d
//      (Manhattan) from the middle are flowing water of level 7 - d, none past 6; nothing spreads off its fall
//  - its see through mesh draws each level's top at its height, (4 + level / 8) blocks: 1 to 6 (the 7 under its
//      fall has water over it, drawn full)
//  - then a brick under the water dug out: the water falls down the hole, to the chunk's bottom
//  - then the source taken out: all of it drains, no water left in the chunk
//  - run: bash bsh/tst.sh water_flow
byte zox_tst_flow_stage = 0;
double zox_tst_flow_time = 0;
entity zox_tst_flow_chunk = 0;
int zox_tst_flow_wrong = -1, zox_tst_flow_outside = -1, zox_tst_flow_above = -1, zox_tst_flow_left = -1;
byte zox_tst_flow_fall = 0, zox_tst_flow_hole = 0, zox_tst_flow_under = 0;
byte zox_tst_flow_heights = 0;  // a bit a level (1 to 7) drawn at its height on the floor's water

// a voxel's water level in the flow chunk, 0 not water
static inline byte zox_tst_flow_level(ecs* world, const WaterLevels* levels, byte3 p) {
    const byte voxel = getv_VoxelNode(zox_get(zox_tst_flow_chunk, VoxelNode), zox_getv(zox_tst_flow_chunk, NodeDepth), p);
    return levels->level[voxel];
}

byte zox_tst_water_flow_ready(ecs* world) {
    zox_tst_world_ready(world);
    const entity terrain = zox_tst_world_terrain;
    if (!terrain) {
        return 0;
    }
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    zox_tst_progress(zox_tst_flow_stage * 100 + (int) (zox_current_time - zox_tst_flow_time));
    if (zox_tst_flow_stage == 0) {
        zox_tst_flow_chunk = zox_tst_streamed_air_chunk(world);
        if (!zox_tst_flow_chunk) {
            return 0;
        }
        const entity bricks = zox_get_child_by_id(world, zox_tst_world_realm, zox_id(BlockBricks));
        VoxelNodeQueue* queue = zox_getm(zox_tst_flow_chunk, VoxelNodeQueue);
        for (int x = 1; x <= 13; x++) for (int z = 1; z <= 13; z++) {
            a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = zox_getv(bricks, BlockIndex), .position = (byte3) { x, 3, z } });
        }
        zox_tst_flow_stage = 1;
        zox_tst_flow_time = zox_current_time;
        return 0;
    }
    if (zox_tst_flow_stage == 1) {
        zox_tst_waiting = "the floor, then the water dropped on it";
        if (zox_current_time - zox_tst_flow_time < 1) {
            return 0;
        }
        const entity water = zox_get_child_by_id(world, zox_tst_world_realm, zox_id(BlockWater));
        VoxelNodeQueue* queue = zox_getm(zox_tst_flow_chunk, VoxelNodeQueue);
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = zox_getv(water, BlockIndex), .position = (byte3) { 7, 6, 7 } });
        zox_tst_flow_stage = 2;
        zox_tst_flow_time = zox_current_time;
        return 0;
    }
    WaterLevels levels;
    water_levels_get(world, zox_tst_world_realm, &levels);
    if (zox_tst_flow_stage == 2) {
        zox_tst_waiting = "the water to fall and spread";
        if (zox_current_time - zox_tst_flow_time < 5) {
            return 0;
        }
        zox_tst_flow_wrong = zox_tst_flow_outside = zox_tst_flow_above = 0;
        for (int x = 1; x <= 13; x++) for (int z = 1; z <= 13; z++) {
            const int d = abs(x - 7) + abs(z - 7);
            const byte level = zox_tst_flow_level(world, &levels, (byte3) { x, 4, z });
            if (d < zox_water_level_source - 1) {
                if (level != zox_water_level_source - 1 - d) {
                    if (!zox_tst_flow_wrong) {
                        zox_log("[tst] water_flow: [%ix4x%i] is level [%i], not [%i]", x, z, level, zox_water_level_source - 1 - d);
                    }
                    zox_tst_flow_wrong++;
                }
            } else {
                zox_tst_flow_outside += level > 0;
            }
            for (int y = 5; y <= 8; y++) {
                zox_tst_flow_above += !(x == 7 && z == 7) && zox_tst_flow_level(world, &levels, (byte3) { x, y, z });
            }
        }
        // the water's mesh: its tops at their levels
        const float scale = zox_getv(zox_tst_world_terrain, BlockScale);
        entity meshes[zox_chunk_meshes_max];
        const uint count = get_chunk_meshes(world, zox_tst_flow_chunk, meshes);
        for (uint i = 0; i < count; i++) {
            if (!zox_has(meshes[i], TransparentMesh) || zox_has(meshes[i], Disabled) || !zox_has(meshes[i], MeshVertices)) {
                continue;
            }
            const MeshVertices* vertices = zox_get(meshes[i], MeshVertices);
            for (int j = 0; j < vertices->length; j++) {
                const float eighths = (vertices->value[j].y / scale - 4) * zox_water_level_source;
                const int level = (int) roundf(eighths);
                if (level >= 1 && level < zox_water_level_source && fabsf(eighths - level) < 0.01f) {
                    zox_tst_flow_heights |= 1 << level;
                }
            }
        }
        zox_tst_flow_fall = zox_tst_flow_level(world, &levels, (byte3) { 7, 5, 7 }) == zox_water_level_source - 1 &&
            zox_tst_flow_level(world, &levels, (byte3) { 7, 6, 7 }) == zox_water_level_source;
        // then a brick dug out from under the water (2 from the middle): the water drains down the hole
        VoxelNodeQueue* queue = zox_getm(zox_tst_flow_chunk, VoxelNodeQueue);
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = 0, .position = (byte3) { 7, 3, 9 } });
        zox_tst_flow_stage = 3;
        zox_tst_flow_time = zox_current_time;
        return 0;
    }
    if (zox_tst_flow_stage == 3) {
        zox_tst_waiting = "the water to fall down the hole";
        if (zox_current_time - zox_tst_flow_time < 3) {
            return 0;
        }
        zox_tst_flow_hole = zox_tst_flow_level(world, &levels, (byte3) { 7, 3, 9 }) > 0;
        zox_tst_flow_under = zox_tst_flow_level(world, &levels, (byte3) { 7, 2, 9 }) > 0 && zox_tst_flow_level(world, &levels, (byte3) { 7, 0, 9 }) > 0;
        // the source taken out: it all drains
        a_VoxelNodeQueue(zox_getm(zox_tst_flow_chunk, VoxelNodeQueue), (VoxelNodeUpdate) { .value = 0, .position = (byte3) { 7, 6, 7 } });
        zox_tst_flow_stage = 4;
        zox_tst_flow_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the water to drain away";
    zox_tst_flow_left = 0;
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        zox_tst_flow_left += zox_tst_flow_level(world, &levels, (byte3) { x, y, z }) > 0;
    }
    return !zox_tst_flow_left || zox_current_time - zox_tst_flow_time > 12;
}

void zox_tst_water_flow_check(ecs* world) {
    zox_tst_check(zox_tst_flow_stage == 4, "no all air chunk at full detail was found");
    if (zox_tst_flow_stage != 4) {
        return;
    }
    zox_log("[tst] [metric] water_flow on the floor: [%i] blocks within 6 of the middle not their level, [%i] past it water; its fall [%i], [%i] water elsewhere over the floor",
        zox_tst_flow_wrong, zox_tst_flow_outside, zox_tst_flow_fall, zox_tst_flow_above);
    zox_tst_check(!zox_tst_flow_wrong, "[%i] blocks within 6 of the middle aren't water a level less a block", zox_tst_flow_wrong);
    zox_tst_check(!zox_tst_flow_outside, "[%i] blocks past 6 of the middle are water: it spread too far", zox_tst_flow_outside);
    zox_tst_check(zox_tst_flow_fall, "the water's fall isn't flowing 7 under its source");
    zox_tst_check(!zox_tst_flow_above, "[%i] water blocks over the floor beside the fall: it spread from its fall", zox_tst_flow_above);
    zox_log("[tst] [metric] water_flow levels drawn at their heights [0x%x] (1 to 6: 0x7e)", zox_tst_flow_heights);
    zox_tst_check(zox_tst_flow_heights == 0x7e, "the water's mesh draws levels [0x%x] at their heights, not 1 to 6 (0x7e)", zox_tst_flow_heights);
    zox_log("[tst] [metric] water_flow the dug hole water [%i], the air under it down the chunk water [%i], [%i] water left after its source went",
        zox_tst_flow_hole, zox_tst_flow_under, zox_tst_flow_left);
    zox_tst_check(zox_tst_flow_hole && zox_tst_flow_under, "the water didn't fall down the dug hole (the hole [%i], under it [%i])",
        zox_tst_flow_hole, zox_tst_flow_under);
    zox_tst_check(!zox_tst_flow_left, "[%i] water blocks left in the chunk after its source was taken out: it didn't drain", zox_tst_flow_left);
}
