// tst: terrain_collisions, bodies against voxel terrain (terrain_collision_system)
//  - builds its own tiny terrain (frame.c), no streaming: two chunks side by side (x -8 to 8, z 0 to 8)
//      - floor: block layers 0 to 3, its top at y = 2
//      - wall: x 6 to 6.5, y 2 to 6, the whole chunk deep, on the right chunk, with two holes 3 blocks tall
//          (y 2 to 3.5): z 5.5 to 6 and z 6.5 to 7
//      - ceiling: x -8 to -6.5, y 6 to 7, on the left chunk
//      - step: a block (x 4 to 4.5, y 2 to 2.5, z 0 to 0.5)
//  - bodies are 0.5 x 1 x 0.5 (half extents 0.25 0.5 0.25) unless said, standing on the floor at y = 2.5
//  - 3s at a fixed 60fps (10 and 30fps in the _10fps and _30fps variants), gravity on, no friction unless said:
//      drop: falls from rest, lands and rests on the floor, Grounded
//      fast: falls at 60 m/s (a metre a frame, more than its half height), doesn't sink in
//      bullet: falls at 400 m/s, its first frame ends 1.7m under the 2m floor, clear of it
//          (so only the sweep can catch it, not the stuck-in-a-block fallback)
//      wall: runs into the wall at 80 m/s (1.3m a frame), stops at its face (Frictioned,
//          so it doesn't bounce back off the terrain)
//      seam: slides over the chunk seam at x = 0 and every block seam, never snags or bobs
//      edge: walks off the end of the terrain (z = 8), falls, Grounded goes leaving -> air
//      buried: spawned inside the floor, comes out on top
//      jump: jumps at 8 m/s, peaks at v^2 / 2g above the floor, lands again
//      thin wall: touching the wall, at 90 m/s its first frame ends clear past the 0.5m wall
//      ceiling: jumps at 40 m/s into a 1m slab overhead (y 6 to 7), never gets through or into
//          it (rising against gravity is where the swept move is guessed least well)
//      step: walked (Frictioned, pushed) into a step a block tall, stays in front of it, never climbs it
//      slide: pushed into the wall while sliding along it, slides at its speed (no snags on the wall's block seams)
//          and never into the wall
//      narrow: 0.6 wide, walked into a hole 0.5 wide, never gets in
//      fits: 0.4 wide, walked into a hole 0.5 wide (in its middle), walks through
//  - and turned (frame.c), the same run with gravity +y, +x and -z: terrain_collisions_up, _x and _z
//  - run: bash bsh/tst.sh terrain_collisions

#define zox_tst_tc_seconds 3.0
#define zox_tst_tc_floor_top 2.0f    // block layers 0 to 3
#define zox_tst_tc_wall_x 6.0f       // block 12
#define zox_tst_tc_step_x 4.0f       // block 8
#define zox_tst_tc_terrain_end_z 8.0f

#define zox_tst_tc_drop 0
#define zox_tst_tc_fast 1
#define zox_tst_tc_bullet 2
#define zox_tst_tc_wall 3
#define zox_tst_tc_seam 4
#define zox_tst_tc_edge 5
#define zox_tst_tc_buried 6
#define zox_tst_tc_jump 7
#define zox_tst_tc_thin_wall 8
#define zox_tst_tc_ceiling 9
#define zox_tst_tc_step 10
#define zox_tst_tc_slide 11
#define zox_tst_tc_narrow 12
#define zox_tst_tc_fits 13
#define zox_tst_tc_count 14

const float zox_tst_tc_rest_y = zox_tst_tc_floor_top + 0.5f;

zox_tst_clock zox_tst_tc_clock;
byte zox_tst_tc_spawned = 0;
entity zox_tst_tc_terrain = 0;
entity zox_tst_tc_entities[zox_tst_tc_count];
zox_tst_tc_body zox_tst_tc_bodies[zox_tst_tc_count] = {
    { "drop", { 2, 6, 4 }, { 0, 0, 0 } },
    { "fast", { 3, 7, 4 }, { 0, -60, 0 } },
    { "bullet", { 4, 5, 4 }, { 0, -400, 0 } },
    { "wall", { 2, 2.51f, 2 }, { 80, 0, 0 }, 1 },
    { "seam", { -3, 2.51f, 6 }, { 2, 0, 0 } },
    { "edge", { 1, 2.51f, 7 }, { 0, 0, 3 } },
    { "buried", { -2, 1.0f, 1 }, { 0, 0, 0 } },
    { "jump", { -5, 2.5f, 4 }, { 0, 8, 0 } },
    { "thin wall", { 5.7f, 2.51f, 5 }, { 90, 0, 0 }, 1 },
    { "ceiling", { -7, 2.5f, 4 }, { 0, 40, 0 } },
    { "step", { 2, 2.51f, 0.25f }, { 0, 0, 0 }, 1, { 10, 0, 0 } },
    { "slide", { 5.7f, 2.51f, 0.5f }, { 0, 0, 1 }, 0, { 5, 0, 0 } },
    { "narrow", { 5, 2.51f, 6.75f }, { 0, 0, 0 }, 1, { 6, 0, 0 }, { 0.3f, 0.5f, 0.3f } },
    { "fits", { 5.3f, 2.51f, 5.75f }, { 0, 0, 0 }, 1, { 4.5f, 0, 0 }, { 0.2f, 0.5f, 0.2f } },
};
// tracked every frame, in the local frame
float zox_tst_tc_lowest[zox_tst_tc_count];     // lowest y once it first touched the floor
float zox_tst_tc_highest[zox_tst_tc_count];    // highest y
float zox_tst_tc_furthest_x[zox_tst_tc_count]; // furthest x
byte zox_tst_tc_landed[zox_tst_tc_count];
float zox_tst_tc_seam_x_error = 0;             // worst |x - (x0 + v t)| on the seam body
float zox_tst_tc_slide_z_error = 0;            // worst |z - (z0 + v t)| on the slide body
byte zox_tst_tc_edge_states[3];                // edge body: saw grounded, leaving, air (in that order)

static inline byte zox_tst_tc_is_solid(const int3 block) {
    if (block.y < 4) {
        return 1;  // floor
    }
    if (block.x >= -16 && block.x <= -13 && (block.y == 12 || block.y == 13)) {
        return 1;  // ceiling, x -8 to -6.5, y 6 to 7
    }
    if (block.x == 8 && block.y == 4 && block.z == 0) {
        return 1;  // step
    }
    if (block.x == 12 && block.y <= 6 && (block.z == 11 || block.z == 13)) {
        return 0;  // holes in the wall
    }
    return block.x == 12 && block.y < 12;  // wall
}

byte zox_tst_terrain_collisions_ready(ecs* world) {
    zox_tst_waiting = "bodies to land on the terrain";
    if (!zox_tst_tc_spawned) {
        if (!zox_tst_clock_prepare(&zox_tst_tc_clock, zox_tst_physics_dt)) {
            return 0;
        }
        zox_tst_tc_spawned = 1;
        const int3 chunks[] = { { 0, 0, 0 }, { -1, 0, 0 } };
        zox_tst_tc_terrain = zox_tst_tc_spawn_terrain(world, chunks, 2, zox_tst_tc_is_solid);
        for (int i = 0; i < zox_tst_tc_count; i++) {
            zox_tst_tc_entities[i] = zox_tst_tc_spawn_body(world, zox_tst_tc_terrain, &zox_tst_tc_bodies[i]);
            zox_tst_tc_lowest[i] = 1000;
            zox_tst_tc_highest[i] = -1000;
            zox_tst_tc_furthest_x[i] = -1000;
        }
        return 0;
    }
    zox_tst_clock_tick(&zox_tst_tc_clock);
    for (int i = 0; i < zox_tst_tc_count; i++) {
        const entity e = zox_tst_tc_entities[i];
        const float3 p = zox_tst_tc_local(zox_getv(e, Position3D));
        if (p.x > zox_tst_tc_furthest_x[i]) {
            zox_tst_tc_furthest_x[i] = p.x;
        }
        if (p.y > zox_tst_tc_highest[i]) {
            zox_tst_tc_highest[i] = p.y;
        }
        if (zox_getv(e, Grounded) == ground_state_grounded) {
            zox_tst_tc_landed[i] = 1;
        }
        if (zox_tst_tc_landed[i] && p.y < zox_tst_tc_lowest[i]) {
            zox_tst_tc_lowest[i] = p.y;
        }
        zox_tst_tc_control(world, e, &zox_tst_tc_bodies[i], (float) zox_tst_tc_clock.dt);
    }
    // seam and slide: slide at their start speed (no friction), so where they are is known every frame
    {
        const zox_tst_tc_body b = zox_tst_tc_bodies[zox_tst_tc_seam];
        const float x = zox_tst_tc_local(zox_getv(zox_tst_tc_entities[zox_tst_tc_seam], Position3D)).x;
        const float error = fabsf(x - (b.position.x + b.velocity.x * (float) zox_tst_tc_clock.time));
        if (error > zox_tst_tc_seam_x_error) {
            zox_tst_tc_seam_x_error = error;
        }
        const zox_tst_tc_body s = zox_tst_tc_bodies[zox_tst_tc_slide];
        const float z = zox_tst_tc_local(zox_getv(zox_tst_tc_entities[zox_tst_tc_slide], Position3D)).z;
        const float slide_error = fabsf(z - (s.position.z + s.velocity.z * (float) zox_tst_tc_clock.time));
        if (slide_error > zox_tst_tc_slide_z_error) {
            zox_tst_tc_slide_z_error = slide_error;
        }
    }
    // edge: grounded, then leaving, then in the air
    {
        const byte state = zox_getv(zox_tst_tc_entities[zox_tst_tc_edge], Grounded);
        if (state == ground_state_grounded) {
            zox_tst_tc_edge_states[0] = 1;
        } else if (state == ground_state_leaving && zox_tst_tc_edge_states[0]) {
            zox_tst_tc_edge_states[1] = 1;
        } else if (state == ground_state_air && zox_tst_tc_edge_states[1]) {
            zox_tst_tc_edge_states[2] = 1;
        }
    }
    return zox_tst_physics_done(zox_tst_tc_clock.time, zox_tst_tc_seconds);
}

void zox_tst_terrain_collisions_check(ecs* world) {
    zox_tst_clock_check(&zox_tst_tc_clock, "terrain_collisions");
    zox_log("[tst] terrain_collisions with [%s]", zox_tst_tc_frames[zox_tst_tc_frame_index].name);
    const float rest = zox_tst_tc_rest_y;
    const float g = -zox_default_gravity.y;
    for (int i = 0; i < zox_tst_tc_count; i++) {
        const char* label = zox_tst_tc_bodies[i].label;
        const entity e = zox_tst_tc_entities[i];
        const float3 p = zox_tst_tc_local(zox_getv(e, Position3D));
        const float3 v = zox_tst_tc_local(zox_getv(e, Velocity3D));
        zox_tst_check(zox_tst_float3_finite(p) && zox_tst_float3_finite(v), "[%s] went NaN or infinite", label);
        if (i == zox_tst_tc_edge) {
            continue;
        }
        // resting on the floor, still, Grounded
        zox_tst_check(fabsf(p.y - rest) < 0.01f, "[%s] ended at y [%f], expected resting on the floor at [%f]", label, p.y, rest);
        zox_tst_check(fabsf(v.y) < 0.2f, "[%s] still moving up or down at [%f] on the floor", label, v.y);
        zox_tst_check(zox_getv(e, Grounded) == ground_state_grounded, "[%s] not Grounded on the floor [%i]", label, zox_getv(e, Grounded));
        // once landed, never sinks into the floor
        if (i != zox_tst_tc_buried) {
            zox_tst_check(zox_tst_tc_lowest[i] > rest - 0.02f,
                "[%s] sank to y [%f] after landing, the floor's at [%f]", label, zox_tst_tc_lowest[i], rest);
        }
    }
    // walls: never went past the face, but got there (then bounced back, bounce_lost_force)
    const int walls[] = { zox_tst_tc_wall, zox_tst_tc_thin_wall, zox_tst_tc_slide, zox_tst_tc_narrow, zox_tst_tc_step };
    for (int w = 0; w < 5; w++) {
        const zox_tst_tc_body* b = &zox_tst_tc_bodies[walls[w]];
        const float x = zox_tst_tc_furthest_x[walls[w]];
        const float face = (walls[w] == zox_tst_tc_step ? zox_tst_tc_step_x : zox_tst_tc_wall_x) - zox_tst_tc_body_half(b).x;
        zox_tst_check(x <= face + 0.01f, "[%s] reached x [%f], into or through the wall, its face is at [%f]", b->label, x, face);
        zox_tst_check(x > face - 0.05f, "[%s] only reached x [%f], stopped short of the wall face [%f]", b->label, x, face);
    }
    // step: never climbed it
    zox_tst_check(zox_tst_tc_highest[zox_tst_tc_step] < rest + 0.02f,
        "[step] rose to y [%f] walking into a step a block tall, the floor's at [%f]", zox_tst_tc_highest[zox_tst_tc_step], rest);
    // slide: along the wall at its speed
    zox_tst_check(zox_tst_tc_slide_z_error < 0.01f,
        "[slide] snagged on the wall, z was off by up to [%f] from sliding freely", zox_tst_tc_slide_z_error);
    // fits: walked through the hole, past the wall
    {
        const float x = zox_tst_tc_local(zox_getv(zox_tst_tc_entities[zox_tst_tc_fits], Position3D)).x;
        const float past = zox_tst_tc_wall_x + 0.5f + zox_tst_tc_bodies[zox_tst_tc_fits].half.x;
        zox_tst_check(x > past, "[fits] at x [%f], didn't walk through the hole, past the wall is [%f]", x, past);
    }
    // ceiling: its head never went above the slab's underside
    {
        const float head = zox_tst_tc_highest[zox_tst_tc_ceiling] + zox_tst_tc_half.y;
        zox_tst_check(head <= 6.0f + 0.01f, "[ceiling] head reached y [%f], into or through the ceiling at [6]", head);
        zox_tst_check(head >= 6.0f - 0.05f, "[ceiling] head only reached y [%f], should have hit the ceiling at [6]", head);
    }
    // seam: no snags over the chunk and block seams
    zox_tst_check(zox_tst_tc_seam_x_error < 0.01f,
        "[seam] snagged, x was off by up to [%f] from sliding freely", zox_tst_tc_seam_x_error);
    zox_tst_check(zox_tst_tc_highest[zox_tst_tc_seam] < rest + 0.02f,
        "[seam] bobbed up to y [%f] sliding over the floor at [%f]", zox_tst_tc_highest[zox_tst_tc_seam], rest);
    // edge: fell off the end, and Grounded went grounded -> leaving -> air
    {
        const entity e = zox_tst_tc_entities[zox_tst_tc_edge];
        const float3 p = zox_tst_tc_local(zox_getv(e, Position3D));
        zox_tst_check(p.z > zox_tst_tc_terrain_end_z && p.y < zox_tst_tc_floor_top - 1,
            "[edge] at [%f %f %f], should have walked off the end of the terrain and fallen", p.x, p.y, p.z);
        zox_tst_check(zox_tst_tc_edge_states[0] && zox_tst_tc_edge_states[1] && zox_tst_tc_edge_states[2],
            "[edge] Grounded went through grounded [%i] leaving [%i] air [%i]",
            zox_tst_tc_edge_states[0], zox_tst_tc_edge_states[1], zox_tst_tc_edge_states[2]);
        zox_tst_check(zox_getv(e, Grounded) == ground_state_air, "[edge] still Grounded [%i] in the air", zox_getv(e, Grounded));
    }
    // jump: peak height v^2 / 2g (exact steps, frames land within g dt^2 / 2 below the top)
    {
        const float v0 = zox_tst_tc_bodies[zox_tst_tc_jump].velocity.y;
        const float peak = rest + v0 * v0 / (2 * g);
        const float height = zox_tst_tc_highest[zox_tst_tc_jump];
        const float dt = (float) zox_tst_tc_clock.dt;
        zox_tst_check(height <= peak + 0.01f && height >= peak - g * dt * dt / 2 - 0.01f,
            "[jump] peaked at y [%f], expected v^2 / 2g above the floor [%f]", height, peak);
    }
}
