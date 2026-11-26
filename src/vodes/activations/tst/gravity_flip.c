// tst: gravity_flip, a character stands on its gravity when a gravity cube flips it (src/terrains/voxel/gravity
//  gravity_upright_system, jumps along its up)
//  - a character (characters/tst/character.c) in a brick room in an air chunk (air_chunk.c, no game, no player): a
//    floor at y 3, a ceiling at y 8 (air 4 blocks tall between), a gravity cube at (8, 6, 6) by it
//  - on the floor facing +x, the cube switched on (block_use): it falls up onto the ceiling, and within a second it's
//    upside down (its up along -y), still facing +x (it rolls over, keeping its heading), Grounded on the ceiling
//  - then a jump: it leaves the ceiling, downward (its up), and lands back on it
//  - run: bash bsh/tst.sh gravity_flip
byte zox_tst_gflip_stage = 0;
double zox_tst_gflip_time = 0;
float3 zox_tst_gflip_up, zox_tst_gflip_forward;
float zox_tst_gflip_ceiling_y = 0, zox_tst_gflip_lowest = 0, zox_tst_gflip_jump_y = 0;
byte zox_tst_gflip_grounded = 0, zox_tst_gflip_landed = 0;
int3 zox_tst_gflip_cube;

static inline byte zox_tst_gflip_at(int x, int y, int z) {
    return (y == 3 || y == 8) && x >= 2 && x <= 13 && z >= 2 && z <= 13;
}

byte zox_tst_gravity_flip_ready(ecs* world) {
    const entity chunk = zox_tst_streamed_air_chunk(world);
    if (!chunk) {
        return 0;
    }
    const entity terrain = zox_tst_world_terrain;
    const float scale = zox_getv(terrain, BlockScale);
    const int length = octree_size(zox_getv(terrain, NodeDepth));
    const int3 c = zox_getv(chunk, ChunkPosition);
    if (zox_tst_gflip_stage == 0) {
        const byte bricks = zox_getv(zox_tst_air_block(world, zox_id(BlockBricks)), BlockIndex);
        const byte cube = zox_getv(zox_tst_air_block(world, zox_id(BlockGravity)), BlockIndex);
        VoxelNodeQueue* queue = zox_getm(chunk, VoxelNodeQueue);
        for (byte x = 2; x <= 13; x++) for (byte y = 3; y <= 8; y++) for (byte z = 2; z <= 13; z++) {
            if (zox_tst_gflip_at(x, y, z)) {
                a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = bricks, .position = (byte3) { x, y, z } });
            }
        }
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = cube, .position = (byte3) { 8, 6, 6 } });
        zox_tst_gflip_cube = (int3) { c.x * length + 8, c.y * length + 6, c.z * length + 6 };
        zox_tst_gflip_ceiling_y = (c.y * length + 8) * scale;
        zox_tst_gflip_stage = 1;
        zox_tst_gflip_time = zox_current_time;
        return 0;
    }
    if (zox_tst_gflip_stage == 1) {
        zox_tst_waiting = "the room to be built";
        if (zox_current_time - zox_tst_gflip_time < 1 || zox_has(chunk, ChunkUpdate)) {
            return 0;
        }
        zox_tst_gflip_stage = 2;
        return 0;
    }
    const entity e = zox_tst_body_ready(world, terrain, zox_tst_air_position(world, chunk, (float3) { 8, 5, 8 }));
    if (!e) {
        return 0;
    }
    const float4 rotation = zox_getv(e, Rotation3D);
    const float3 p = zox_getv(e, Position3D);
    const float half = physics3_half_along(zox_getv(e, Bounds3D), float3_up);
    if (zox_tst_gflip_stage == 2) {
        zox_tst_waiting = "the character to land on the floor";
        if (zox_getv(e, Grounded) != ground_state_grounded) {
            return 0;
        }
        zox_setv(e, Rotation3D, quaternion_from_axis_angle(float3_up, -90 * degreesToRadians));   // facing +x
        zox_tst_gflip_forward = float4_rotate_float3(quaternion_from_axis_angle(float3_up, -90 * degreesToRadians), (float3) { 0, 0, 1 });
        block_use(world, 0, terrain, zox_tst_world_realm, zox_tst_gflip_cube);
        zox_tst_gflip_stage = 3;
        zox_tst_gflip_time = zox_current_time;
        return 0;
    }
    if (zox_tst_gflip_stage == 3) {
        zox_tst_waiting = "the character to stand on the ceiling";
        if (zox_current_time - zox_tst_gflip_time < 1.5) {
            return 0;
        }
        zox_tst_gflip_up = float4_rotate_float3(rotation, float3_up);
        const float3 forward = float4_rotate_float3(rotation, (float3) { 0, 0, 1 });
        zox_tst_gflip_forward = (float3) { float3_dot(forward, zox_tst_gflip_forward), 0, 0 };   // its x: how far it kept facing
        zox_tst_gflip_grounded = zox_getv(e, Grounded) == ground_state_grounded;
        zox_tst_gflip_jump_y = p.y + half;      // its feet on the ceiling
        zox_tst_gflip_lowest = p.y;
        zox_setv(e, JumpState, jump_state_trigger);
        zox_tst_gflip_stage = 4;
        zox_tst_gflip_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "its jump from the ceiling";
    if (p.y < zox_tst_gflip_lowest) {
        zox_tst_gflip_lowest = p.y;
    }
    if (zox_current_time - zox_tst_gflip_time > 0.3 && zox_getv(e, Grounded) == ground_state_grounded) {
        zox_tst_gflip_landed = 1;
    }
    return zox_tst_gflip_landed || zox_current_time - zox_tst_gflip_time > 3;
}

void zox_tst_gravity_flip_check(ecs* world) {
    const float scale = zox_getv(zox_tst_world_terrain, BlockScale);
    const float3 u = zox_tst_gflip_up;
    zox_log("[tst] gravity_flip: up [%.2f %.2f %.2f], facing kept [%.2f], grounded [%i], feet [%.3f] the ceiling [%.3f], "
        "jumped [%.2f] blocks down, landed [%i]", u.x, u.y, u.z, zox_tst_gflip_forward.x, zox_tst_gflip_grounded,
        zox_tst_gflip_jump_y, zox_tst_gflip_ceiling_y, (zox_tst_gflip_jump_y - zox_tst_gflip_lowest) / scale, zox_tst_gflip_landed);
    zox_tst_check(zox_tst_gflip_stage == 4, "the test stopped at stage [%i]", zox_tst_gflip_stage);
    zox_tst_check(u.y < -0.99f, "it isn't upside down, its up [%.2f %.2f %.2f]", u.x, u.y, u.z);
    zox_tst_check(zox_tst_gflip_forward.x > 0.99f, "it turned from facing +x, [%.2f] of it kept", zox_tst_gflip_forward.x);
    zox_tst_check(zox_tst_gflip_grounded, "it isn't Grounded on the ceiling");
    zox_tst_check(fabsf(zox_tst_gflip_jump_y - zox_tst_gflip_ceiling_y) < 0.05f * scale,
        "its feet at y [%.3f], not on the ceiling's underside [%.3f]", zox_tst_gflip_jump_y, zox_tst_gflip_ceiling_y);
    zox_tst_check(zox_tst_gflip_jump_y - zox_tst_gflip_lowest > 0.5f * scale, "its jump took it [%.2f] blocks from the ceiling, "
        "not down (its up)", (zox_tst_gflip_jump_y - zox_tst_gflip_lowest) / scale);
    zox_tst_check(zox_tst_gflip_landed, "it didn't land back on the ceiling");
}
