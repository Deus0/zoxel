// tst: footsteps, the player walking on the ground plays footsteps (characters/jump/sys/footsteps.c)
//  - a character (characters/tst/character.c) on a brick floor in an air chunk (no game, no player), pushed along for a few seconds (its velocity, as if walking), it's grounded
//  - its FootstepDistance adds up and footsteps play, the block underfoot's hit sound (doc/sounds.md)
//  - run: bash bsh/tst.sh footsteps
double zox_tst_footsteps_start = 0;
uint zox_tst_footsteps_before = 0;
#define zox_tst_footsteps_seconds 3.0

byte zox_tst_footsteps_floor = 0;
double zox_tst_footsteps_floor_time = 0;

byte zox_tst_footsteps_ready(ecs* world) {
    // a brick floor in an all air chunk (air_chunk.c), then a character on it (character.c): no game, no player
    const entity chunk = zox_tst_streamed_air_chunk(world);
    if (!chunk) {
        return 0;
    }
    if (!zox_tst_footsteps_floor) {
        const entity bricks = zox_tst_air_block(world, zox_id(BlockBricks));
        VoxelNodeQueue* queue = zox_getm(chunk, VoxelNodeQueue);
        for (int x = 1; x <= 14; x++) for (int z = 1; z <= 14; z++) {
            a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = zox_getv(bricks, BlockIndex), .position = (byte3) { x, 3, z } });
        }
        zox_tst_footsteps_floor = 1;
        zox_tst_footsteps_floor_time = zox_current_time;
        return 0;
    }
    if (zox_current_time - zox_tst_footsteps_floor_time < 1) {
        return 0;
    }
    const entity character = zox_tst_body_ready(world, zox_tst_world_terrain, zox_tst_air_position(world, chunk, (float3) { 2, 5, 7 }));
    if (!character) {
        return 0;
    }
    if (!zox_has(character, Footsteps)) {
        zox_add(character, Footsteps);
        zox_setv(character, FootstepDistance, 0);
    }
    // landed first
    if (!zox_tst_footsteps_start && zox_getv(character, Grounded) != ground_state_grounded) {
        zox_tst_waiting = "the character to land on the floor";
        return 0;
    }
    if (!zox_tst_footsteps_start) {
        zox_tst_footsteps_start = zox_current_time;
        zox_tst_footsteps_before = zox_footsteps_played;
    }
    zox_tst_waiting = "the character to walk";
    // walking: a steady push along x, the ground's friction would stop a single one
    float3 velocity = zox_getv(character, Velocity3D);
    velocity.x = 3;
    zox_setv(character, Velocity3D, velocity);
    return zox_current_time - zox_tst_footsteps_start >= zox_tst_footsteps_seconds;
}

void zox_tst_footsteps_check(ecs* world) {
    const entity character = zox_tst_body;
    const uint steps = zox_footsteps_played - zox_tst_footsteps_before;
    zox_log("[tst] [metric] footsteps [%u] in [%.0fs], grounded [%i]", steps, zox_tst_footsteps_seconds, zox_getv(character, Grounded));
    zox_tst_check(zox_has(character, Footsteps), "the character has no Footsteps tag");
    zox_tst_check(steps >= 2, "only [%u] footsteps walking for [%.0fs]", steps, zox_tst_footsteps_seconds);
}

void zox_tsts_footsteps(ecs* world) {
    zox_tst_add("footsteps", zox_tst_footsteps_ready, zox_tst_footsteps_check, 120);
    zox_tst_set_setup("footsteps", NULL, 1);
}
