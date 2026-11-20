// tst: auto_jump, with the Auto Jump setting on, a player walking into a one block step jumps up onto it
//  (controllers3/sys/auto_jump.c)
//  - on the test level: a solid block put two blocks along +x of its feet, air over it, then walked along +x (its
//    velocity, as doorway does) for a few seconds
//  - it climbed: its feet a block higher, past the step's near side; and with the setting off it doesn't jump
//  - run: bash bsh/tst.sh auto_jump
byte zox_tst_auto_jump_stage = 0;
double zox_tst_auto_jump_time = 0;
int3 zox_tst_auto_jump_step;
float zox_tst_auto_jump_start_y = 0, zox_tst_auto_jump_y = 0, zox_tst_auto_jump_x = 0;

static inline int3 zox_tst_auto_jump_feet(ecs* world, entity character, float scale) {
    const float3 p = zox_getv(character, Position3D);
    const float3 b = zox_getv(character, Bounds3D);
    return real_position_to_block_position((float3) { p.x, p.y - b.y + scale * 0.5f, p.z }, scale);
}

byte zox_tst_auto_jump_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity terrain = local_terrain;
    const float scale = zox_getv(terrain, BlockScale);
    const double t = zox_current_time - zox_tst_auto_jump_time;
    zox_tst_progress(zox_tst_auto_jump_stage * 100 + t * 10);
    if (zox_tst_auto_jump_stage == 0) {
        const int3 feet = zox_tst_auto_jump_feet(world, character, scale);
        const int ground = terrain_voxel_at(world, terrain, int3_add(feet, int3_down));
        zox_tst_check(ground > 0, "no ground under the player [%i]", ground);
        if (ground <= 0) {
            return 1;
        }
        // a step two blocks along +x, ground under the way to it, air over both
        zox_tst_auto_jump_step = (int3) { feet.x + 2, feet.y, feet.z };
        for (int x = 0; x <= 3; x++) {
            terrain_queue_voxel(world, terrain, (int3) { feet.x + x, feet.y - 1, feet.z }, ground, 0);
            for (int y = 0; y <= 3; y++) {
                const int3 at = { feet.x + x, feet.y + y, feet.z };
                terrain_queue_voxel(world, terrain, at, x >= 2 && y == 0 ? ground : 0, 0);
            }
        }
        float3 position = zox_getv(character, Position3D);
        position.z = (feet.z + 0.5f) * scale;
        position.x = (feet.x + 0.5f) * scale;
        zox_setv(character, Position3D, position);
        auto_jump = 1;
        zox_tst_auto_jump_stage = 1;
        zox_tst_auto_jump_time = zox_current_time;
        return 0;
    }
    if (zox_tst_auto_jump_stage == 1) {
        zox_tst_waiting = "the step to be placed";
        if (t < 1 || terrain_voxel_at(world, terrain, zox_tst_auto_jump_step) <= 0) {
            return 0;
        }
        zox_tst_auto_jump_start_y = zox_getv(character, Position3D).y / scale;
        zox_tst_auto_jump_stage = 2;
        zox_tst_auto_jump_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the player to walk up the step";
    float3 velocity = zox_getv(character, Velocity3D);
    velocity.x = 1.5f;
    velocity.z = 0;
    zox_setv(character, Velocity3D, velocity);
    zox_tst_auto_jump_y = zox_getv(character, Position3D).y / scale;
    zox_tst_auto_jump_x = zox_getv(character, Position3D).x / scale;
    return zox_tst_auto_jump_x > zox_tst_auto_jump_step.x + 0.5f || t > 4;
}

void zox_tst_auto_jump_check(ecs* world) {
    (void) world;
    auto_jump = 0;
    zox_log("[tst] auto_jump: from y [%.2f] to [%.2f], x [%.2f], the step at x [%i]",
        zox_tst_auto_jump_start_y, zox_tst_auto_jump_y, zox_tst_auto_jump_x, zox_tst_auto_jump_step.x);
    zox_tst_check(zox_tst_auto_jump_stage == 2, "the test stopped at stage [%i]", zox_tst_auto_jump_stage);
    zox_tst_check(zox_tst_auto_jump_y - zox_tst_auto_jump_start_y > 0.8f, "didn't climb the step, up [%.2f] blocks",
        zox_tst_auto_jump_y - zox_tst_auto_jump_start_y);
    zox_tst_check(zox_tst_auto_jump_x > zox_tst_auto_jump_step.x, "stuck at x [%.2f], the step's at [%i]",
        zox_tst_auto_jump_x, zox_tst_auto_jump_step.x);
}

void zox_tsts_auto_jump(ecs* world) {
    (void) world;
    zox_tst_add("auto_jump", zox_tst_auto_jump_ready, zox_tst_auto_jump_check, 90);
    zox_tst_set_setup("auto_jump", zox_tst_level_setup, 0);
}
