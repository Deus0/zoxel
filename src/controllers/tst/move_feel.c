// tst: move_feel, how a player's walking feels in numbers, kept through the movement refactor (doc/plans/movement.md)
//  - the test level: flat land, one chunk, no towns, npcs, plants or mountains, a player character alone; first
//    person, zoxel's speeds (gam/zoxel/set); the fake keyboard (tests read no real keys)
//  - each move from standing, held [zox_tst_mf_hold] seconds then let go: forward (W), back (S), right (D), left (A),
//    running (W and shift); crouching is tst crouch
//  - each: its top speed (the flat speed averaged over its last second), how long it took to reach 90% of it, and how
//    far it slid once let go
//  - every frame 1/60s (zox_delta_time_override), time counted in those, seed 666 (tst.sh seeded_tests): the same
//    numbers every run, any machine. At software rendering's ~0.07s frames the old move strafed in pulses (a frame's
//    push near its top speed, held back till friction stopped it)
//  - checks: each near the old move's (zox_tst_mf_feel, 2026-11-27, before the refactor): top speed 5%, time to it
//    0.15s and slide 0.1m, each two frames more (the keys reach the move a frame or two late, longer at 10fps)
//  - move_feel_10fps: the same at 0.1s frames, the same numbers (the move is exact at any frame rate)
//  - run: bash bsh/tst.sh move_feel
#define zox_tst_mf_hold 2.5
#define zox_tst_mf_moves 5
const char* zox_tst_mf_names[zox_tst_mf_moves] = { "forward", "back", "right", "left", "run" };
// the old move's feel with zoxel's speeds: top speed m/s, seconds to 90% of it, metres slid stopping
const float3 zox_tst_mf_feel[zox_tst_mf_moves] = {
    { 1.281f, 0.150f, 0.198f },     // forward
    { 1.283f, 0.167f, 0.227f },     // back
    { 1.066f, 0.117f, 0.173f },     // right
    { 1.102f, 0.117f, 0.204f },     // left
    { 2.134f, 0.333f, 0.306f },     // run
};
int zox_tst_mf_move = 0;
byte zox_tst_mf_stage = 0;      // 0 settling, 1 holding, 2 stopping
double zox_tst_mf_time = 0;       // simulated seconds into this stage
double zox_tst_mf_dt = 1.0 / 60.0;    // a frame (move_feel_10fps: 0.1)
float3 zox_tst_mf_let_go;
float zox_tst_mf_sum[zox_tst_mf_moves], zox_tst_mf_t90[zox_tst_mf_moves], zox_tst_mf_slide[zox_tst_mf_moves];
int zox_tst_mf_samples[zox_tst_mf_moves];
float zox_tst_mf_speeds[1024];  // this move's flat speed each frame, and when
float zox_tst_mf_times[1024];
int zox_tst_mf_count = 0;

static inline void zox_tst_mf_keys(ecs* world, int move, byte held) {
    entity devices[zox_children_capacity];
    const uint length = zox_get_children_by_id(world, zox_players[0], devices, zox_children_capacity, zox_id(Device));
    for (uint i = 0; i < length; i++) {
        if (!zox_has(devices[i], Keyboard)) {
            continue;
        }
        Keyboard keys = { 0 };
        if (held) {
            keys.w.is_pressed = move == 0 || move == 4;
            keys.s.is_pressed = move == 1;
            keys.d.is_pressed = move == 2;
            keys.a.is_pressed = move == 3;
            keys.left_shift.is_pressed = move == 4;
        }
        zox_set_ptr(devices[i], Keyboard, keys);
    }
}

static inline float zox_tst_mf_flat_speed(ecs* world, entity character) {
    const float3 v = zox_getv(character, Velocity3D);
    return sqrtf(v.x * v.x + v.z * v.z);
}

byte zox_tst_move_feel_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    if (!zox_valid(character) || !zox_getv(character, Grounded)) {
        zox_tst_waiting = "the player on the ground";
        return 0;
    }
    if (zox_tst_mf_move == zox_tst_mf_moves) {
        zox_delta_time_override_count = 0;
        zox_delta_time_override = 0;
        return 1;
    }
    zox_delta_time_override = zox_tst_mf_dt;
    zox_delta_time_override_count = 2;     // this frame and the next (as physics/tst/common.c)
    zox_tst_mf_time += zox_tst_mf_dt;
    const int m = zox_tst_mf_move;
    const double t = zox_tst_mf_time;
    const float speed = zox_tst_mf_flat_speed(world, character);
    zox_tst_progress(m * 1000 + zox_tst_mf_stage * 100 + (int) (t * 10));
    if (zox_tst_mf_stage == 0) {
        zox_tst_waiting = "standing still";
        zox_tst_mf_keys(world, m, 0);
        if (speed < 0.01f) {
            zox_tst_mf_stage = 1;
            zox_tst_mf_time = 0;
            zox_tst_mf_count = 0;
        }
        return 0;
    }
    if (zox_tst_mf_stage == 1) {
        zox_tst_waiting = "a move held";
        zox_tst_mf_keys(world, m, 1);
        if (zox_tst_mf_count < 1024) {
            zox_tst_mf_speeds[zox_tst_mf_count] = speed;
            zox_tst_mf_times[zox_tst_mf_count++] = t;
        }
        if (t > zox_tst_mf_hold - 1) {
            zox_tst_mf_sum[m] += speed;
            zox_tst_mf_samples[m]++;
        }
        if (t >= zox_tst_mf_hold) {
            const float top = zox_tst_mf_sum[m] / int_max(1, zox_tst_mf_samples[m]);
            zox_tst_mf_t90[m] = -1;
            for (int i = 0; i < zox_tst_mf_count; i++) {
                if (zox_tst_mf_speeds[i] >= top * 0.9f) {
                    zox_tst_mf_t90[m] = zox_tst_mf_times[i];
                    break;
                }
            }
            zox_tst_mf_keys(world, m, 0);
            zox_tst_mf_let_go = zox_getv(character, Position3D);
            zox_tst_mf_stage = 2;
            zox_tst_mf_time = 0;
        }
        return 0;
    }
    zox_tst_waiting = "it to stop sliding";
    if (speed < 0.01f || t > 3) {
        const float3 p = zox_getv(character, Position3D);
        zox_tst_mf_slide[m] = sqrtf((p.x - zox_tst_mf_let_go.x) * (p.x - zox_tst_mf_let_go.x) +
            (p.z - zox_tst_mf_let_go.z) * (p.z - zox_tst_mf_let_go.z));
        zox_tst_mf_move++;
        zox_tst_mf_stage = 0;
    }
    return 0;
}

// move_feel at 10 frames a second: the same numbers as at 60 (the move is exact at any frame rate; the old one
//  strafed in pulses at slow frames)
byte zox_tst_move_feel_10fps_ready(ecs* world) {
    zox_tst_mf_dt = 0.1;
    return zox_tst_move_feel_ready(world);
}

void zox_tst_move_feel_check(ecs* world) {
    (void) world;
    for (int m = 0; m < zox_tst_mf_moves; m++) {
        const float top = zox_tst_mf_sum[m] / int_max(1, zox_tst_mf_samples[m]);
        zox_log("[tst] [metric] move_feel [%s]: top speed [%.3f] m/s, to 90%% in [%.3f]s, slid [%.3f]m stopping",
            zox_tst_mf_names[m], top, zox_tst_mf_t90[m], zox_tst_mf_slide[m]);
        const float3 feel = zox_tst_mf_feel[m];
        if (!feel.x) {
            continue;
        }
        zox_tst_check(fabsf(top - feel.x) <= feel.x * 0.05f, "[%s] top speed [%.3f], was [%.3f]", zox_tst_mf_names[m], top, feel.x);
        // the keys reach the move a frame or two after they're set: two frames more at slow frames
        const float frames = 2 * (float) zox_tst_mf_dt;
        zox_tst_check(fabsf(zox_tst_mf_t90[m] - feel.y) <= 0.15f + frames, "[%s] reached 90%% in [%.3f]s, was [%.3f]s",
            zox_tst_mf_names[m], zox_tst_mf_t90[m], feel.y);
        zox_tst_check(fabsf(zox_tst_mf_slide[m] - feel.z) <= 0.1f + frames * top, "[%s] slid [%.3f]m stopping, was [%.3f]m",
            zox_tst_mf_names[m], zox_tst_mf_slide[m], feel.z);
    }
}

void zox_tsts_move_feel(ecs* world) {
    (void) world;
    zox_tst_add("move_feel", zox_tst_move_feel_ready, zox_tst_move_feel_check, 120);
    zox_tst_set_setup("move_feel", zox_tst_level_setup, 0);
    zox_tst_add("move_feel_10fps", zox_tst_move_feel_10fps_ready, zox_tst_move_feel_check, 120);
    zox_tst_set_setup("move_feel_10fps", zox_tst_level_setup, 0);
}
