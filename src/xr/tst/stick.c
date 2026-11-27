// tst: xr_stick, walking by the left stick in a headset (doc/tech/vr.md, Moving by the stick): the view moves at the
//  stick's speed at once, steady, stopped at once; the body walks after it by the move
//  - into a game on the simulator (as xr_body), then each from standing:
//  - at a headset's 72 fps and at 90 fps (fixed frames): the view's speed each frame within 3% of its top speed
//    walking, under 0.05 3 frames after the stick's let go
//  - turning by the right stick while walking (72 fps): the view's speed still steady, no frame's move past its step
//    (the lead's in the world: a turn mustn't jolt it)
//  - turning on the spot by the right stick: the eyes stay put, the view turns about the head (it turned the body about
//    its middle, the eyes ahead of it swung sideways ~0.3m/s, the lead takes it back)
//  - a wall 1m ahead (bricks), walked into 3s: the body stops at it, the eyes never in a block, nor 5cm ahead of them
//    (the near plane) every frame, the lead at most its max (it led the eyes 21cm into a wall before it was kept
//    clear of blocks; the wall's face on the walk's line is only a guide, on the grid at an angle it's a staircase)
//  - run: bash bsh/tst.sh xr_stick
#define zox_tst_xs_phases 5
const char* zox_tst_xs_names[zox_tst_xs_phases] = { "72fps", "90fps", "turning", "turning still", "wall" };
const double zox_tst_xs_dts[zox_tst_xs_phases] = { 1.0 / 72.0, 1.0 / 90.0, 1.0 / 72.0, 1.0 / 72.0, 1.0 / 72.0 };
int zox_tst_xs_phase = 0;
int zox_tst_xs_frame = 0;
float3 zox_tst_xs_eyes_last;
float zox_tst_xs_min[zox_tst_xs_phases], zox_tst_xs_max[zox_tst_xs_phases], zox_tst_xs_stopped[zox_tst_xs_phases];
float zox_tst_xs_top = 0;
// the wall phase
float3 zox_tst_xs_from, zox_tst_xs_dir;
float zox_tst_xs_face = -1;         // the wall's near face along the walk, at eye height
float zox_tst_xs_eyes_most = -1;    // the eyes' furthest along it
float zox_tst_xs_body_moved = -1;
float zox_tst_xs_lead_most = 0;
int zox_tst_xs_eyes_in = 0;         // frames the eyes (or just ahead of them, the near plane) were in a block
byte zox_tst_xs_wall_built = 0;

// a wall of bricks ahead, 1m to 1.75m, 2m wide, under the feet to 2.5m up (as xr_body's)
static inline byte zox_tst_xs_wall(ecs* world, entity character) {
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    const entity bricks = zox_valid(realm) ? zox_get_child_by_id(world, realm, zox_id(BlockBricks)) : 0;
    if (!zox_valid(terrain) || !zox_valid(bricks)) {
        return 0;
    }
    const float scale = zox_getv(terrain, BlockScale);
    const byte index = zox_getv(bricks, BlockIndex);
    const float3 side = float3_cross(float3_up, zox_tst_xs_dir);
    int placed = 0;
    for (float ahead = 1.0f; ahead <= 1.75f; ahead += scale * 0.5f) {
        for (float across = -1; across <= 1; across += scale * 0.5f) {
            for (float up = -1; up <= 2.5f; up += scale * 0.5f) {
                const float3 p = float3_add(zox_tst_xs_from, float3_add(float3_add(
                    float3_scale(zox_tst_xs_dir, ahead), float3_scale(side, across)), (float3) { 0, up, 0 }));
                placed += terrain_queue_voxel(world, terrain, real_position_to_block_position(p, scale), index, 0);
            }
        }
    }
    return placed > 0;
}

// the wall's near face along the walk at the eyes' height, probing the terrain each cm (-1 none found)
static inline float zox_tst_xs_wall_face(ecs* world, entity character, float eye_height) {
    const entity terrain = zox_get_link(world, character, TerrainLink);
    if (!zox_valid(terrain)) {
        return -1;
    }
    const float scale = zox_getv(terrain, BlockScale);
    for (float d = 0.3f; d < 2.0f; d += 0.01f) {
        float3 p = float3_add(zox_tst_xs_from, float3_scale(zox_tst_xs_dir, d));
        p.y = eye_height;
        if (terrain_voxel_at(world, terrain, real_position_to_block_position(p, scale)) > 0) {
            return d;
        }
    }
    return -1;
}

byte zox_tst_xr_stick_ready(ecs* world) {
    if (zox_tst_xr_play_stage < 3) {
        zox_tst_xr_play_ready(world);
        return 0;
    }
    const entity character = xr_body_character(world);
    if (!zox_valid(character) || !xr_body_head(world, xr_camera) || !zox_getv(character, Grounded)) {
        zox_tst_waiting = "the headset's body on the ground";
        return 0;
    }
    if (zox_tst_xs_phase == zox_tst_xs_phases) {
        zox_delta_time_override_count = 0;
        zox_delta_time_override = 0;
        xr_sim_script_left_stick = float2_zero;
        xr_sim_script_stick = float2_zero;
        return 1;
    }
    const int p = zox_tst_xs_phase;
    const int f = zox_tst_xs_frame++;
    const float dt = (float) zox_tst_xs_dts[p];
    zox_delta_time_override = dt;
    zox_delta_time_override_count = 2;
    zox_tst_waiting = zox_tst_xs_names[p];
    zox_tst_progress(p * 1000 + f);
    const float3 eyes = zox_tst_xr_body_eyes();
    const float3 d = float3_sub(eyes, zox_tst_xs_eyes_last);
    const float speed = f > 0 ? sqrtf(d.x * d.x + d.z * d.z) / dt : 0;
    zox_tst_xs_eyes_last = eyes;
    const byte wall = p == 4;
    const byte still = p == 3;      // turning on the spot: the stick's turn only
    const int walk_end = wall ? 280 : 120;     // frames the stick's held to (from 30)
    if (f == 0) {
        zox_tst_xs_min[p] = 1000;
        zox_tst_xs_max[p] = 0;
        zox_tst_xs_stopped[p] = 1000;
        zox_tst_xs_top = character3_top_speed(world, character).y;
        if (wall) {
            zox_tst_xs_from = zox_getv(character, Position3D);
            zox_tst_xs_dir = float3_normalize(float4_rotate_float3(xr_body_rotation(world, character), (float3) { 0, 0, -1 }));
            zox_tst_xs_dir.y = 0;
            zox_tst_xs_dir = float3_normalize(zox_tst_xs_dir);
            zox_tst_xs_wall_built = zox_tst_xs_wall(world, character);
        }
        return 0;
    }
    if (wall && f == 25) {
        zox_tst_xs_face = zox_tst_xs_wall_face(world, character, eyes.y);
    }
    // standing still till 30, the stick held to walk_end, then let go
    xr_sim_script_left_stick = !still && f >= 30 && f < walk_end ? (float2) { 0, 1 } : float2_zero;
    xr_sim_script_stick = (p == 2 && f >= 60 && f < 100) || (still && f >= 30 && f < walk_end) ? (float2) { 1, 0 } : float2_zero;
    // walking: from 3 frames in, while it's held (the view's speed this frame)
    if (!wall && f >= 33 && f < walk_end) {
        zox_tst_xs_min[p] = float_min(zox_tst_xs_min[p], speed);
        zox_tst_xs_max[p] = float_max(zox_tst_xs_max[p], speed);
    }
    if (!wall && f == walk_end + 3) {
        zox_tst_xs_stopped[p] = speed;
    }
    if (wall && f > 30) {
        const float along = float3_dot(float3_sub(eyes, zox_tst_xs_from), zox_tst_xs_dir);
        zox_tst_xs_eyes_most = float_max(zox_tst_xs_eyes_most, along);
        // the eyes, and 5cm ahead of them along the walk (the near plane), in the air
        const entity terrain = zox_get_link(world, character, TerrainLink);
        if (zox_valid(terrain)) {
            const float scale = zox_getv(terrain, BlockScale);
            const float3 ahead = float3_add(eyes, float3_scale(zox_tst_xs_dir, 0.05f));
            zox_tst_xs_eyes_in += terrain_voxel_at(world, terrain, real_position_to_block_position(eyes, scale)) > 0 ||
                terrain_voxel_at(world, terrain, real_position_to_block_position(ahead, scale)) > 0;
        }
        zox_tst_xs_lead_most = float_max(zox_tst_xs_lead_most, float3_length(xr_stick_lead));
        zox_tst_xs_body_moved = float3_dot(float3_sub(zox_getv(character, Position3D), zox_tst_xs_from), zox_tst_xs_dir);
    }
    if (f >= walk_end + 60) {
        zox_tst_xs_phase++;
        zox_tst_xs_frame = 0;
    }
    return 0;
}

void zox_tst_xr_stick_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] xr_stick [turning still]: the eyes moved at most [%.3f] m/s", zox_tst_xs_max[3]);
    zox_tst_check(zox_tst_xs_max[3] < 0.05f, "turning on the spot the eyes moved [%.3f] m/s (it turns about the body, not the head)", zox_tst_xs_max[3]);
    for (int p = 0; p < 3; p++) {
        zox_log("[tst] [metric] xr_stick [%s]: the view's speed walking [%.3f to %.3f] of [%.3f], 3 frames after let go [%.3f]",
            zox_tst_xs_names[p], zox_tst_xs_min[p], zox_tst_xs_max[p], zox_tst_xs_top, zox_tst_xs_stopped[p]);
        zox_tst_check(zox_tst_xs_min[p] > zox_tst_xs_top * 0.97f && zox_tst_xs_max[p] < zox_tst_xs_top * 1.03f,
            "[%s] the view's speed wasn't steady [%.3f to %.3f] of [%.3f]", zox_tst_xs_names[p], zox_tst_xs_min[p], zox_tst_xs_max[p], zox_tst_xs_top);
        zox_tst_check(zox_tst_xs_stopped[p] < 0.05f, "[%s] let go the view coasted [%.3f]", zox_tst_xs_names[p], zox_tst_xs_stopped[p]);
    }
    zox_log("[tst] [metric] xr_stick [wall]: its face [%.3f] ahead on the walk's line, the eyes got [%.3f], the body [%.3f], the lead at most [%.3f], the eyes in a block [%i] frames",
        zox_tst_xs_face, zox_tst_xs_eyes_most, zox_tst_xs_body_moved, zox_tst_xs_lead_most, zox_tst_xs_eyes_in);
    zox_tst_check(zox_tst_xs_wall_built && zox_tst_xs_face > 0, "no wall found ahead (built [%i])", zox_tst_xs_wall_built);
    zox_tst_check(zox_tst_xs_body_moved > 0.1f && zox_tst_xs_body_moved < 1.0f, "the body went [%.3f] at a wall 1m ahead",
        zox_tst_xs_body_moved);
    // the body's own eyes may reach past it (its collisions are shallower than its eyes' reach: the fade, doc/tech/vr.md,
    //  isn't done), the lead never takes them further
    zox_tst_check(zox_tst_xs_eyes_in == 0, "the eyes saw into the wall [%i] frames", zox_tst_xs_eyes_in);
    zox_tst_check(zox_tst_xs_lead_most <= 0.251f, "the view led the body by [%.3f], past its max", zox_tst_xs_lead_most);
}

void zox_tsts_xr_stick(ecs* world) {
    (void) world;
    zox_tst_add("xr_stick", zox_tst_xr_stick_ready, zox_tst_xr_stick_check, 150);
    zox_tst_set_setup("xr_stick", zox_tst_xr_sim_setup, 1);
}
