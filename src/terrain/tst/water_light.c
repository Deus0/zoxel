// tst: water_light, sunlight dims through water, light_water_decay a block (lights3: sunbeam, dark_sunbeam, editing)
//  - no player: in an all air chunk (air_chunk.c) under the sky, a brick pool 3 x 3 inside, 3 deep (local y 4 to 6)
//  - filled a layer at a time from the bottom (each placed water dims the beam under it): its middle column is one
//      light_dim less each block down, the air over it full sunlight
//  - its top layer taken out (drained): the two left are lit a block brighter
//  - a brick over its middle (in the beam): the column under it is darker than the beam was
//  - run: bash bsh/tst.sh water_light
byte zox_tst_wlight_stage = 0;
double zox_tst_wlight_time = 0;
entity zox_tst_wlight_chunk = 0;
byte zox_tst_wlight_full[4];     // the middle column, local y 7 (air) down to 4, the pool full
byte zox_tst_wlight_drained[4];  // its top layer taken out
byte zox_tst_wlight_covered[4];  // a brick over it at y 7

static inline void zox_tst_wlight_read(ecs* world, byte* lights) {
    const byte depth = zox_getv(zox_tst_wlight_chunk, NodeDepth);
    for (byte i = 0; i < 4; i++) {
        lights[i] = getv_LightNode(zox_get(zox_tst_wlight_chunk, LightNode), depth, (byte3) { 7, 7 - i, 7 });
    }
}

// queues a layer of the pool's inside (6 to 8) at y as a block
static inline void zox_tst_wlight_layer(ecs* world, byte y, byte value) {
    VoxelNodeQueue* queue = zox_getm(zox_tst_wlight_chunk, VoxelNodeQueue);
    for (byte x = 6; x <= 8; x++) for (byte z = 6; z <= 8; z++) {
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = value, .position = (byte3) { x, y, z } });
    }
}

byte zox_tst_water_light_ready(ecs* world) {
    zox_tst_world_ready(world);
    if (!zox_tst_world_terrain) {
        return 0;
    }
    zox_tst_progress(zox_tst_wlight_stage * 100 + (int) (zox_current_time - zox_tst_wlight_time));
    const double since = zox_current_time - zox_tst_wlight_time;
    const byte water = zox_getv(zox_tst_air_block(world, zox_id(BlockWater)), BlockIndex);
    const byte bricks = zox_getv(zox_tst_air_block(world, zox_id(BlockBricks)), BlockIndex);
    switch (zox_tst_wlight_stage) {
        case 0: {
            zox_tst_wlight_chunk = zox_tst_streamed_air_chunk(world);
            if (!zox_tst_wlight_chunk) {
                return 0;
            }
            // the floor (y 3) and walls (y 4 to 6) round the inside, 6 to 8
            VoxelNodeQueue* queue = zox_getm(zox_tst_wlight_chunk, VoxelNodeQueue);
            for (byte x = 5; x <= 9; x++) for (byte z = 5; z <= 9; z++) for (byte y = 3; y <= 6; y++) {
                const byte inside = x >= 6 && x <= 8 && z >= 6 && z <= 8;
                if (y == 3 || !inside) {
                    a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = bricks, .position = (byte3) { x, y, z } });
                }
            }
            break;
        }
        case 1: case 2: case 3:
            zox_tst_waiting = "the pool to fill a layer at a time";
            if (since < 1) {
                return 0;
            }
            zox_tst_wlight_layer(world, 3 + zox_tst_wlight_stage, water);
            break;
        case 4:
            zox_tst_waiting = "the full pool's light";
            if (since < 2) {
                return 0;
            }
            zox_tst_wlight_read(world, zox_tst_wlight_full);
            zox_tst_wlight_layer(world, 6, 0);
            break;
        case 5:
            zox_tst_waiting = "the drained pool's light";
            if (since < 2) {
                return 0;
            }
            zox_tst_wlight_read(world, zox_tst_wlight_drained);
            a_VoxelNodeQueue(zox_getm(zox_tst_wlight_chunk, VoxelNodeQueue), (VoxelNodeUpdate) { .value = bricks, .position = (byte3) { 7, 7, 7 } });
            break;
        case 6:
            zox_tst_waiting = "the covered pool's light";
            if (since < 2) {
                return 0;
            }
            zox_tst_wlight_read(world, zox_tst_wlight_covered);
            break;
        default:
            return 1;
    }
    zox_tst_wlight_stage++;
    zox_tst_wlight_time = zox_current_time;
    return 0;
}

void zox_tst_water_light_check(ecs* world) {
    zox_tst_check(zox_tst_wlight_stage == 7, "no all air chunk at full detail was found");
    if (zox_tst_wlight_stage != 7) {
        return;
    }
    const byte* f = zox_tst_wlight_full;
    const byte* d = zox_tst_wlight_drained;
    const byte* c = zox_tst_wlight_covered;
    zox_log("[tst] [metric] water_light the middle column y 7 to 4: full [%i %i %i %i], drained [%i %i %i %i], covered [%i %i %i %i]",
        f[0], f[1], f[2], f[3], d[0], d[1], d[2], d[3], c[0], c[1], c[2], c[3]);
    zox_tst_check(f[0] == sunlight, "the air over the pool is lit [%i], not full sunlight", f[0]);
    for (byte i = 1; i < 4; i++) {
        zox_tst_check(f[i] == light_dim(f[i - 1], darklight), "full: the water [%i] down is lit [%i], not [%i] (a block dimmer)",
            i, f[i], light_dim(f[i - 1], darklight));
    }
    zox_tst_check(d[0] == sunlight && d[1] == sunlight, "drained: the air over the water is lit [%i, %i], not full sunlight", d[0], d[1]);
    zox_tst_check(d[2] == light_dim(sunlight, darklight) && d[3] == light_dim(d[2], darklight),
        "drained: the water left is lit [%i, %i], not [%i, %i]", d[2], d[3], light_dim(sunlight, darklight), light_dim(light_dim(sunlight, darklight), darklight));
    zox_tst_check(c[1] < d[1] && c[2] < d[2] && c[3] < d[3], "covered: under the brick isn't darker [%i %i %i] than before [%i %i %i]",
        c[1], c[2], c[3], d[1], d[2], d[3]);
}
