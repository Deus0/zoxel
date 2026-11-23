// tst: terrain_ledges, a body as big as a character (0.4 x 0.984 x 0.4, a body 1.969 blocks tall in 0.5 blocks) gets
//  into gaps 2 blocks tall and 1 wide (a house's door) by walking and jumping, pushed as the player's controls push
//  (a force up to a top speed, Frictioned), terrain collisions only (no characters: tst step_gap is the same with one)
//  - its own terrain (frame.c), two chunks (x -8 to 8, z 0 to 8), the floor's top at y 2:
//      - step gap: a step a block up (x 4 to 7, y 2 to 2.5, z 0 to 4, a house's floor), a wall over its front edge
//          (x 4 to 4.5, y 2.5 to 4.5) with gaps over the step (y 2.5 to 3.5) at z 1.5 to 2 and z 3 to 3.5
//      - flat gap: a wall (x -3 to -2.5, y 2 to 4) with a gap (y 2 to 3) at z 4 to 4.5
//  - 4s at a fixed 60fps (10 and 30fps in the _10fps and _30fps variants), gravity on:
//      step: walked at the step gap's middle, jumping (6 m/s, as a character's jump) when it's been grounded 0.2s against
//          the step (jump held): lands on the step, walks in past the wall (and stops pushing past it)
//      step off: the same, a tenth of a metre (a fifth of a block) off the gap's middle
//      flat off: walked at the flat gap a tenth of a metre off its middle, walks through
//  - and turned (frame.c) with gravity +x: terrain_ledges_x
//  - run: bash bsh/tst.sh terrain_ledges

#define zox_tst_tl_seconds 4.0
#define zox_tst_tl_step 0
#define zox_tst_tl_step_off 1
#define zox_tst_tl_flat_off 2
#define zox_tst_tl_count 3
#define zox_tst_tl_half_y 0.4922f

zox_tst_clock zox_tst_tl_clock;
byte zox_tst_tl_spawned = 0;
entity zox_tst_tl_entities[zox_tst_tl_count];
zox_tst_tc_body zox_tst_tl_bodies[zox_tst_tl_count] = {
    { "step", { 2, 2.6f, 1.75f }, { 0, 0, 0 }, 1, { 19, 0, 0 }, { 0.2f, zox_tst_tl_half_y, 0.2f }, 1.4f, 6, 3.7f, 5 },
    { "step off", { 2, 2.6f, 3.35f }, { 0, 0, 0 }, 1, { 19, 0, 0 }, { 0.2f, zox_tst_tl_half_y, 0.2f }, 1.4f, 6, 3.7f, 5 },
    { "flat off", { -5, 2.6f, 4.35f }, { 0, 0, 0 }, 1, { 19, 0, 0 }, { 0.2f, zox_tst_tl_half_y, 0.2f }, 1.4f, 0, 0, -2 },
};
// where each has to get to: past its wall (x), its feet on this floor (y)
const float zox_tst_tl_past_x[zox_tst_tl_count] = { 4.7f, 4.7f, -2.3f };
const float zox_tst_tl_floor[zox_tst_tl_count] = { 2.5f, 2.5f, 2.0f };

static inline byte zox_tst_tl_is_solid(const int3 block) {
    if (block.y < 4) {
        return 1;  // floor
    }
    if (block.x >= 8 && block.x <= 13 && block.z >= 0 && block.z <= 7) {
        if (block.y == 4) {
            return 1;  // the step
        }
        // its wall, gaps at z blocks 3 and 6
        return block.x == 8 && block.y <= 8 && !(block.y <= 6 && (block.z == 3 || block.z == 6));
    }
    // the flat wall, a gap at z block 8
    return block.x == -6 && block.y <= 7 && !(block.y <= 5 && block.z == 8);
}

byte zox_tst_terrain_ledges_ready(ecs* world) {
    zox_tst_waiting = "bodies to get into the gaps";
    if (!zox_tst_tl_spawned) {
        if (!zox_tst_clock_prepare(&zox_tst_tl_clock, zox_tst_physics_dt)) {
            return 0;
        }
        zox_tst_tl_spawned = 1;
        const int3 chunks[] = { { 0, 0, 0 }, { -1, 0, 0 } };
        const entity terrain = zox_tst_tc_spawn_terrain(world, chunks, 2, zox_tst_tl_is_solid);
        for (int i = 0; i < zox_tst_tl_count; i++) {
            zox_tst_tl_entities[i] = zox_tst_tc_spawn_body(world, terrain, &zox_tst_tl_bodies[i]);
        }
        return 0;
    }
    zox_tst_clock_tick(&zox_tst_tl_clock);
    for (int i = 0; i < zox_tst_tl_count; i++) {
        zox_tst_tc_control(world, zox_tst_tl_entities[i], &zox_tst_tl_bodies[i], (float) zox_tst_tl_clock.dt);
    }
    return zox_tst_physics_done(zox_tst_tl_clock.time, zox_tst_tl_seconds);
}

void zox_tst_terrain_ledges_check(ecs* world) {
    zox_tst_clock_check(&zox_tst_tl_clock, "terrain_ledges");
    zox_log("[tst] terrain_ledges with [%s]", zox_tst_tc_frames[zox_tst_tc_frame_index].name);
    for (int i = 0; i < zox_tst_tl_count; i++) {
        const char* label = zox_tst_tl_bodies[i].label;
        const float3 p = zox_tst_tc_local(zox_getv(zox_tst_tl_entities[i], Position3D));
        const float feet = p.y - zox_tst_tl_half_y;
        zox_log("[tst] [%s] at [%.3f %.3f %.3f], feet at [%.3f]", label, p.x, p.y, p.z, feet);
        zox_tst_check(zox_tst_float3_finite(p), "[%s] went NaN or infinite", label);
        zox_tst_check(p.x > zox_tst_tl_past_x[i], "[%s] stuck at x [%.3f], didn't get through the gap (past it is [%.2f])",
            label, p.x, zox_tst_tl_past_x[i]);
        zox_tst_check(fabsf(feet - zox_tst_tl_floor[i]) < 0.02f, "[%s] its feet at y [%.3f], expected on the floor at [%.2f]",
            label, feet, zox_tst_tl_floor[i]);
    }
}
