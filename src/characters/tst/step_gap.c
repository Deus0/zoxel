// tst: step_gap, a character jumps up a block into a gap 2 blocks tall (as into a house: its floor a block up, a wall
//  over the gap), terrain collisions only (no door; the body is 1.969 blocks tall: tst doorway)
//  - a character (character.c) on a brick floor in an air chunk (air_chunk.c, no game, no player): the floor at y 3,
//    a step a block higher (y 4, x 8..13) and a wall across x 8 (z 2..13, y 5..8) with a gap at z 7, y 5 and 6
//    (air 2 blocks tall, solid under and over it)
//  - walked at it along +x, pushed as the player's controls push (player3d_move_system: a force up to its top speed),
//    facing it, at the gap's middle (zox_tst_step_gap_off=<blocks> off it)
//  - once against the step, jump held (JumpState triggered whenever CanJump, as the player's controls do;
//    zox_tst_step_gap_run=1 holds it all the way, a running jump)
//  - it's in: past the wall (x over 9.5), standing on the step (its feet at y 5)
//  - the bug it caught: pressed against the wall over the step, it fell past the step's top (its feet have 0.03 blocks
//    of room) and the push-out sent it back off the step every time; now it's lifted onto it (terrain_ledges)
//  - zox_tst_step_gap_log=1 logs it every frame
//  - run: bash bsh/tst.sh step_gap
byte zox_tst_step_gap_stage = 0;
double zox_tst_step_gap_time = 0;
float zox_tst_step_gap_x = 0, zox_tst_step_gap_y = 0, zox_tst_step_gap_top = 0;
int zox_tst_step_gap_jumps = 0;

static inline byte zox_tst_step_gap_at(int x, int y, int z) {
    if (y == 3) {
        return x >= 1 && x <= 14 && z >= 1 && z <= 14;
    }
    if (y == 4) {
        return x >= 8 && x <= 13 && z >= 2 && z <= 13;
    }
    return x == 8 && y >= 5 && y <= 8 && z >= 2 && z <= 13 && !(z == 7 && y <= 6);
}

byte zox_tst_step_gap_ready(ecs* world) {
    const entity chunk = zox_tst_streamed_air_chunk(world);
    if (!chunk) {
        return 0;
    }
    const entity terrain = zox_tst_world_terrain;
    if (zox_tst_step_gap_stage == 0) {
        const byte bricks = zox_getv(zox_tst_air_block(world, zox_id(BlockBricks)), BlockIndex);
        VoxelNodeQueue* queue = zox_getm(chunk, VoxelNodeQueue);
        for (byte x = 1; x <= 14; x++) for (byte y = 3; y <= 8; y++) for (byte z = 1; z <= 14; z++) {
            if (zox_tst_step_gap_at(x, y, z)) {
                a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = bricks, .position = (byte3) { x, y, z } });
            }
        }
        zox_tst_step_gap_stage = 1;
        zox_tst_step_gap_time = zox_current_time;
        return 0;
    }
    if (zox_tst_step_gap_stage == 1) {
        zox_tst_waiting = "the step and wall to be built";
        if (zox_current_time - zox_tst_step_gap_time < 1 || zox_has(chunk, ChunkUpdate)) {
            return 0;
        }
        zox_tst_step_gap_stage = 2;
        return 0;
    }
    const entity e = zox_tst_body_ready(world, terrain, zox_tst_air_position(world, chunk, (float3) { 4, 5, 7 }));
    if (!e) {
        return 0;
    }
    const float scale = zox_getv(terrain, BlockScale);
    const int3 c = zox_getv(chunk, ChunkPosition);
    const int length = octree_size(zox_getv(terrain, NodeDepth));
    const float3 p = zox_getv(e, Position3D);
    const float3 b = zox_getv(e, Bounds3D);
    zox_tst_step_gap_x = p.x / scale - c.x * length;
    zox_tst_step_gap_y = (p.y - b.y) / scale - c.y * length;    // its feet
    if (zox_tst_step_gap_stage == 2) {
        zox_tst_waiting = "the character to land on the floor";
        if (zox_getv(e, Grounded) != ground_state_grounded) {
            return 0;
        }
        float3 position = p;
        // the gap's middle, or zox_tst_step_gap_off=<blocks> off it, as a player walks up
        const float off = getenv("zox_tst_step_gap_off") ? atof(getenv("zox_tst_step_gap_off")) : 0;
        position.z = zox_tst_air_position(world, chunk, (float3) { 0, 0, 7 + off }).z;
        zox_setv(e, Position3D, position);
        zox_setv(e, Rotation3D, quaternion_from_euler((float3) { 0, -90 * degreesToRadians, 0 }));
        zox_tst_step_gap_stage = 3;
        zox_tst_step_gap_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the character to jump into the gap";
    if (zox_tst_step_gap_y > zox_tst_step_gap_top) {
        zox_tst_step_gap_top = zox_tst_step_gap_y;
    }
    if (getenv("zox_tst_step_gap_log")) {
        const float3 v = zox_getv(e, Velocity3D);
        zox_log("[tst] step_gap x [%.3f] feet [%.3f] head [%.3f] v [%.2f %.2f] grounded [%i] can jump [%i]", zox_tst_step_gap_x,
            zox_tst_step_gap_y, zox_tst_step_gap_y + b.y * 2 / scale, v.x, v.y, zox_getv(e, Grounded), zox_getv(e, CanJump));
    }
    // up against the step first (stopped by it), then jump held, as a player walks up and jumps
    //  (zox_tst_step_gap_run=1: jump held all the way, a running jump)
    const byte against = zox_tst_step_gap_x > 7.6f || getenv("zox_tst_step_gap_run");
    if (against && zox_getv(e, CanJump)) {
        zox_setv(e, JumpState, jump_state_trigger);
        zox_tst_step_gap_jumps++;
    }
    // pushed forward as the player's controls do (player3d_move_system): a force, up to its top speed
    //  (controllers3/set: max_velocity3D.y 1.4, player_movement_power.y 19, a later module)
    if (zox_getv(e, Velocity3D).x < 1.4f) {
        zox_getm(e, Acceleration3D)->value.x += 19;
    }
    return zox_tst_step_gap_x > 10.5f || zox_current_time - zox_tst_step_gap_time > 5;
}

void zox_tst_step_gap_check(ecs* world) {
    const entity e = zox_tst_body;
    const float3 bounds = zox_valid(e) ? zox_getv(e, Bounds3D) : float3_zero;
    const float scale = zox_getv(zox_tst_world_terrain, BlockScale);
    zox_log("[tst] [metric] step_gap at x [%.2f] feet y [%.2f] (highest [%.2f]), [%i] jumps, the body [%.3f] tall, block scale [%.3f]",
        zox_tst_step_gap_x, zox_tst_step_gap_y, zox_tst_step_gap_top, zox_tst_step_gap_jumps, bounds.y * 2 / scale, scale);
    zox_tst_check(zox_tst_step_gap_stage == 3, "the test stopped at stage [%i]", zox_tst_step_gap_stage);
    zox_tst_check(zox_tst_step_gap_x > 9.5f, "stuck at x [%.2f], the wall is at 8 to 9", zox_tst_step_gap_x);
    zox_tst_check(zox_tst_step_gap_y > 4.9f, "its feet at y [%.2f], not on the step (5)", zox_tst_step_gap_y);
}

void zox_tsts_step_gap(ecs* world) {
    zox_tst_add("step_gap", zox_tst_step_gap_ready, zox_tst_step_gap_check, 120);
    zox_tst_set_setup("step_gap", NULL, 1);
}
