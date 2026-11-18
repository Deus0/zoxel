// tst: doorway, a character walks through a doorway 1 block wide and 2 tall (a body fits: tst body_views)
//  - a character (character.c) on a brick floor in an air chunk (air_chunk.c, no game, no player): a brick wall across x 8
//    (z 2..13, y 4..7) with a doorway at z 7, y 4 and 5 (the 2 blocks over the floor)
//  - walked at it along +x (its velocity, as footsteps does) for a few seconds, facing it, at the doorway's middle
//    (zox_tst_doorway_off=0.1: a tenth of a block off it)
//  - it's through: past the wall (x over 9), on the floor still
//  - run: bash bsh/tst.sh doorway (and with your settings, Block Res 5: zox_tst_settings=~/.config/zoxel/settings.cfg)
byte zox_tst_doorway_stage = 0;
double zox_tst_doorway_time = 0;
float zox_tst_doorway_x = 0, zox_tst_doorway_start_x = 0;
float zox_tst_doorway_off = 7;  // the doorway's middle (block 7), zox_tst_doorway_off=<blocks> walks up off it

static inline byte zox_tst_doorway_at(int x, int y, int z) {
    if (y == 3) {
        return x >= 1 && x <= 14 && z >= 1 && z <= 14;
    }
    return x == 8 && y >= 4 && y <= 7 && z >= 2 && z <= 13 && !(z == 7 && y <= 5);
}

byte zox_tst_doorway_ready(ecs* world) {
    const entity chunk = zox_tst_streamed_air_chunk(world);
    if (!chunk) {
        return 0;
    }
    const entity terrain = zox_tst_world_terrain;
    if (zox_tst_doorway_stage == 0) {
        const byte bricks = zox_getv(zox_tst_air_block(world, zox_id(BlockBricks)), BlockIndex);
        VoxelNodeQueue* queue = zox_getm(chunk, VoxelNodeQueue);
        for (byte x = 1; x <= 14; x++) for (byte y = 3; y <= 7; y++) for (byte z = 1; z <= 14; z++) {
            if (zox_tst_doorway_at(x, y, z)) {
                a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = bricks, .position = (byte3) { x, y, z } });
            }
        }
        zox_tst_doorway_stage = 1;
        zox_tst_doorway_time = zox_current_time;
        return 0;
    }
    if (zox_tst_doorway_stage == 1) {
        zox_tst_waiting = "the wall to be built";
        if (zox_current_time - zox_tst_doorway_time < 1 || zox_has(chunk, ChunkUpdate)) {
            return 0;
        }
        zox_tst_doorway_stage = 2;
        return 0;
    }
    const entity e = zox_tst_body_ready(world, terrain, zox_tst_air_position(world, chunk, (float3) { 4, 5, 7 }));
    if (!e) {
        return 0;
    }
    const float scale = zox_getv(terrain, BlockScale);
    const int3 c = zox_getv(chunk, ChunkPosition);
    const int length = octree_size(zox_getv(terrain, NodeDepth));
    zox_tst_doorway_x = zox_getv(e, Position3D).x / scale - c.x * length;
    if (zox_tst_doorway_stage == 2) {
        zox_tst_waiting = "the character to land on the floor";
        if (zox_getv(e, Grounded) != ground_state_grounded) {
            return 0;
        }
        // facing +x, at the doorway's middle (or off it)
        if (getenv("zox_tst_doorway_off")) {
            zox_tst_doorway_off = 7 + atof(getenv("zox_tst_doorway_off"));
        }
        float3 position = zox_getv(e, Position3D);
        position.z = zox_tst_air_position(world, chunk, (float3) { 0, 0, zox_tst_doorway_off }).z;   // off the middle, as a player walks up
        zox_setv(e, Position3D, position);
        zox_setv(e, Rotation3D, quaternion_from_euler((float3) { 0, -90 * degreesToRadians, 0 }));
        zox_tst_doorway_start_x = zox_tst_doorway_x;
        zox_tst_doorway_stage = 3;
        zox_tst_doorway_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the character to walk through the doorway";
    float3 velocity = zox_getv(e, Velocity3D);
    velocity.x = 2;
    velocity.z = 0;
    zox_setv(e, Velocity3D, velocity);
    return zox_tst_doorway_x > 11 || zox_current_time - zox_tst_doorway_time > 5;
}

void zox_tst_doorway_check(ecs* world) {
    const entity e = zox_tst_body;
    const float3 bounds = zox_valid(e) ? zox_getv(e, Bounds3D) : float3_zero;
    const float scale = zox_getv(zox_tst_world_terrain, BlockScale);
    zox_log("[tst] [metric] doorway walked from x [%.1f] to [%.1f], the body [%.3f] tall [%.3f] wide (blocks)",
        zox_tst_doorway_start_x, zox_tst_doorway_x, bounds.y * 2 / scale, float_max(bounds.x, bounds.z) * 2 / scale);
    zox_tst_check(zox_tst_doorway_stage == 3, "the test stopped at stage [%i]", zox_tst_doorway_stage);
    zox_tst_check(zox_tst_doorway_x > 9.5f, "stuck at x [%.1f], the wall is at 8 to 9", zox_tst_doorway_x);
}

void zox_tsts_doorway(ecs* world) {
    zox_tst_add("doorway", zox_tst_doorway_ready, zox_tst_doorway_check, 120);
    zox_tst_set_setup("doorway", NULL, 1);
}
