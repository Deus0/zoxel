// tst: water_flow, water falls and spreads, so far (terrain/sys/water_flow.c)
//  - no player: in an all air chunk (air_chunk.c), a
//      floor of bricks 13 x 13 (local y 3), then water 2 blocks over its middle
//  - it falls to the floor (local y 4) and spreads over it, as far as zox_water_flow_steps: the blocks in that
//      reach of the middle (Manhattan) are water at y 4, none past it; its fall stays water
//  - then a brick under the water dug out: the water drains down the hole, to the chunk's bottom
//  - run: bash bsh/tst.sh water_flow
byte zox_tst_flow_stage = 0;
double zox_tst_flow_time = 0;
entity zox_tst_flow_chunk = 0;

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
    if (zox_tst_flow_stage == 2) {
        zox_tst_waiting = "the water to fall and spread";
        if (zox_current_time - zox_tst_flow_time < 5) {
            return 0;
        }
        // then a brick dug out from under the water (2 from the middle): the water drains down the hole
        VoxelNodeQueue* queue = zox_getm(zox_tst_flow_chunk, VoxelNodeQueue);
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = 0, .position = (byte3) { 7, 3, 9 } });
        zox_tst_flow_stage = 3;
        zox_tst_flow_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the water to drain down the hole";
    (void) length;
    return zox_current_time - zox_tst_flow_time > 3;
}

void zox_tst_water_flow_check(ecs* world) {
    zox_tst_check(zox_tst_flow_stage == 3, "no all air chunk at full detail was found");
    if (zox_tst_flow_stage != 3) {
        return;
    }
    const entity terrain = zox_tst_world_terrain;
    const byte depth = zox_getv(terrain, NodeDepth);
    const VoxelNode* voxels = zox_get(zox_tst_flow_chunk, VoxelNode);
    const int water_id = zox_tst_sea_water_id(world, terrain);
    int inside = 0, inside_water = 0, outside_water = 0;
    for (int x = 1; x <= 13; x++) for (int z = 1; z <= 13; z++) {
        const byte is_water = getv_VoxelNode(voxels, depth, (byte3) { x, 4, z }) == water_id;
        if (abs(x - 7) + abs(z - 7) <= zox_water_flow_steps) {
            inside++;
            inside_water += is_water;
        } else {
            outside_water += is_water;
        }
    }
    const byte fall = getv_VoxelNode(voxels, depth, (byte3) { 7, 5, 7 }) == water_id && getv_VoxelNode(voxels, depth, (byte3) { 7, 6, 7 }) == water_id;
    int above = 0;
    for (int x = 1; x <= 13; x++) for (int z = 1; z <= 13; z++) for (int y = 5; y <= 8; y++) {
        above += !(x == 7 && z == 7) && getv_VoxelNode(voxels, depth, (byte3) { x, y, z }) == water_id;
    }
    zox_log("[tst] [metric] water_flow on the floor: [%i] of [%i] in reach water, [%i] past it; its fall water [%i], [%i] elsewhere over the floor",
        inside_water, inside, outside_water, fall, above);
    zox_tst_check(inside_water == inside, "only [%i] of the [%i] blocks within [%i] of the middle filled", inside_water, inside, zox_water_flow_steps);
    zox_tst_check(!outside_water, "[%i] blocks past [%i] of the middle filled: it spread too far", outside_water, zox_water_flow_steps);
    zox_tst_check(fall, "the water's fall isn't water");
    zox_tst_check(!above, "[%i] water blocks over the floor beside the fall: it spread before landing", above);
    // the dug hole and the air under it filled
    const byte hole = getv_VoxelNode(voxels, depth, (byte3) { 7, 3, 9 }) == water_id;
    const byte under = getv_VoxelNode(voxels, depth, (byte3) { 7, 2, 9 }) == water_id && getv_VoxelNode(voxels, depth, (byte3) { 7, 0, 9 }) == water_id;
    zox_log("[tst] [metric] water_flow the dug hole water [%i], the air under it down the chunk water [%i]", hole, under);
    zox_tst_check(hole && under, "the water didn't drain down the dug hole (the hole [%i], under it [%i])", hole, under);
}
