// tst: swim, a character in deep water sinks slowly, swims up holding jump, jumps out onto a shore (characters/jump/sys/swim.c)
//  - a character (character.c) dropped into a pool in an air chunk (air_chunk.c, no game, no player): water x z 4..11,
//    y 3..10, a brick floor (y 2) and walls (x z 12, y 2..11); land as the shore at x 0..3 up to y 11 (one over the water)
//  - it falls in: InWater, then it sinks slower than 1m a second (the water holds it up, drags it)
//  - jump held (JumpState triggered whenever CanJump, as the player's controls do): it rises to the surface, not far over it
//  - then by the shore, walking at it with jump held: it climbs out, standing on the shore, out of the water
//  - and the camera's check (terrain_water_at): in the pool's middle, not over it
//  - run: bash bsh/tst.sh swim
byte zox_tst_swim_stage = 0;
double zox_tst_swim_time = 0;
entity zox_tst_swim_chunk = 0;
byte zox_tst_swim_was_in = 0, zox_tst_swim_left = 0, zox_tst_swim_out = 0, zox_tst_swim_grounded = 0;
byte zox_tst_swim_view_in = 0, zox_tst_swim_view_over = 1;
float zox_tst_swim_sink = 0, zox_tst_swim_top = -1000, zox_tst_swim_end_x = 0;

// local to the chunk: 1 brick, 2 water, 0 air
static inline byte zox_tst_swim_at(int x, int y, int z) {
    if (x <= 3 && y >= 2 && y <= 11 && z >= 3 && z <= 12) {
        return 1;
    }
    if (x < 3 || x > 12 || z < 3 || z > 12 || y < 2 || y > 11) {
        return 0;
    }
    if (y == 2 || x == 12 || z == 3 || z == 12) {
        return 1;
    }
    return y <= 10 ? 2 : 0;
}

// holding jump: a trigger whenever it can (player3d_jump_system)
static inline void zox_tst_swim_hold_jump(ecs* world, entity e) {
    if (zox_getv(e, CanJump)) {
        zox_setv(e, JumpState, jump_state_trigger);
    }
}

byte zox_tst_swim_ready(ecs* world) {
    const entity chunk = zox_tst_streamed_air_chunk(world);
    if (!chunk) {
        return 0;
    }
    const entity terrain = zox_tst_world_terrain;
    const float scale = zox_getv(terrain, BlockScale);
    if (zox_tst_swim_stage == 0) {
        zox_tst_swim_chunk = chunk;
        const byte bricks = zox_getv(zox_tst_air_block(world, zox_id(BlockBricks)), BlockIndex);
        const byte water = zox_getv(zox_tst_air_block(world, zox_id(BlockWater)), BlockIndex);
        VoxelNodeQueue* queue = zox_getm(chunk, VoxelNodeQueue);
        for (byte x = 0; x <= 12; x++) for (byte y = 2; y <= 11; y++) for (byte z = 3; z <= 12; z++) {
            const byte at = zox_tst_swim_at(x, y, z);
            if (at) {
                a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = at == 1 ? bricks : water, .position = (byte3) { x, y, z } });
            }
        }
        zox_tst_swim_stage = 1;
        zox_tst_swim_time = zox_current_time;
        return 0;
    }
    if (zox_tst_swim_stage == 1) {
        zox_tst_waiting = "the pool to be built";
        if (zox_current_time - zox_tst_swim_time < 1 || zox_has(chunk, ChunkUpdate)) {
            return 0;
        }
        zox_tst_swim_view_in = terrain_water_at(world, terrain, zox_tst_air_position(world, chunk, (float3) { 8, 6, 8 }));
        zox_tst_swim_view_over = terrain_water_at(world, terrain, zox_tst_air_position(world, chunk, (float3) { 8, 13, 8 }));
        zox_tst_swim_stage = 2;
        return 0;
    }
    const entity e = zox_tst_body_ready(world, terrain, zox_tst_air_position(world, chunk, (float3) { 8, 14, 8 }));
    if (!e) {
        return 0;
    }
    const float3 position = zox_getv(e, Position3D);
    if (zox_tst_swim_stage == 2) {
        zox_tst_waiting = "the character to fall in";
        if (!zox_has(e, InWater)) {
            return 0;
        }
        zox_tst_swim_was_in = 1;
        zox_tst_swim_stage = 3;
        zox_tst_swim_time = zox_current_time;
        return 0;
    }
    if (zox_tst_swim_stage == 3) {
        zox_tst_waiting = "the character to sink";
        if (zox_current_time - zox_tst_swim_time < 3) {
            return 0;
        }
        zox_tst_swim_sink = zox_getv(e, Velocity3D).y;
        zox_tst_swim_stage = 4;
        zox_tst_swim_time = zox_current_time;
        return 0;
    }
    if (zox_tst_swim_stage == 4) {
        zox_tst_waiting = "the character to swim up";
        zox_tst_swim_hold_jump(world, e);
        if (zox_current_time - zox_tst_swim_time > 1.5) {
            zox_tst_swim_top = float_max(zox_tst_swim_top, position.y);
            zox_tst_swim_left |= !zox_has(e, InWater);
        }
        if (zox_current_time - zox_tst_swim_time < 5) {
            return 0;
        }
        // by the shore, at the surface
        zox_setv(e, Position3D, zox_tst_air_position(world, chunk, (float3) { 4, 10.5f, 8 }));
        zox_setv(e, Velocity3D, float3_zero);
        zox_tst_swim_stage = 5;
        zox_tst_swim_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the character to climb out onto the shore";
    const int3 c = zox_getv(chunk, ChunkPosition);
    const int length = octree_size(zox_getv(terrain, NodeDepth));
    zox_tst_swim_end_x = position.x / scale - c.x * length;
    // jump held till it's on the shore, then let go (held, it hopped on the land: not grounded when looked at)
    if (zox_tst_swim_end_x > 2) {
        zox_tst_swim_hold_jump(world, e);
    }
    if (zox_current_time - zox_tst_swim_time > 3) {
        zox_tst_swim_grounded |= zox_getv(e, Grounded) != 0 && zox_tst_swim_end_x < 4;
    }
    // walking at the shore (-x), until it's on it (the land is 4 wide)
    float3 velocity = zox_getv(e, Velocity3D);
    velocity.x = zox_tst_swim_end_x > 2 ? -1.5f : 0;
    zox_setv(e, Velocity3D, velocity);
    if (zox_current_time - zox_tst_swim_time < 4) {
        return 0;
    }
    zox_tst_swim_out = !zox_has(e, InWater);
    return 1;
}

void zox_tst_swim_check(ecs* world) {
    const entity terrain = zox_tst_world_terrain;
    const float scale = zox_getv(terrain, BlockScale);
    const float surface = zox_tst_air_position(world, zox_tst_swim_chunk, (float3) { 0, 10.5f, 0 }).y;   // the water's top
    zox_log("[tst] [metric] swim sinking [%.2f]m/s, swimming up to [%.2f] blocks over the surface, left the water [%i], climbed out [%i] grounded [%i] at x [%.1f]",
        zox_tst_swim_sink, (zox_tst_swim_top - surface) / scale, zox_tst_swim_left, zox_tst_swim_out, zox_tst_swim_grounded, zox_tst_swim_end_x);
    zox_tst_check(zox_tst_swim_view_in && !zox_tst_swim_view_over, "terrain_water_at: in the pool [%i], over it [%i]", zox_tst_swim_view_in, zox_tst_swim_view_over);
    zox_tst_check(zox_tst_swim_stage == 5, "the test stopped at stage [%i]", zox_tst_swim_stage);
    zox_tst_check(zox_tst_swim_was_in, "the character never got InWater");
    zox_tst_check(zox_tst_swim_sink < 0 && zox_tst_swim_sink > -1, "sinking at [%.2f]m/s, not slowly", zox_tst_swim_sink);
    zox_tst_check(zox_tst_swim_top > surface - scale && zox_tst_swim_top < surface + 3 * scale,
        "swimming up it reached [%.2f] blocks over the surface", (zox_tst_swim_top - surface) / scale);
    zox_tst_check(zox_tst_swim_out && zox_tst_swim_grounded && zox_tst_swim_end_x < 4,
        "not out on the shore: out [%i] grounded [%i] at x [%.1f]", zox_tst_swim_out, zox_tst_swim_grounded, zox_tst_swim_end_x);
}

void zox_tsts_swim(ecs* world) {
    zox_tst_add("swim", zox_tst_swim_ready, zox_tst_swim_check, 120);
    zox_tst_set_setup("swim", NULL, 1);
}
