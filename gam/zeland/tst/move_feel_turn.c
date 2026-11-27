// tst: move_feel_turn, turning by physics (the default, player_turn_instant 0): the orbit camera turned round, W held,
//  how long the character takes to face the camera's way (within 5 degrees), and how far past it it swings
//  - zeland: third person, its orbit camera (zoxel's test level is first person); no npcs or towns, a player alone
//  - turns of 90 degrees right, 90 left, 180; every frame 1/60s, seed 666 (as move_feel, src/controllers/tst)
//  - checks: each turn done in under 0.5s, swinging past at most 10 degrees
//  - run: bash bsh/tst.sh move_feel_turn
#define zox_tst_mft_turns 3
const float zox_tst_mft_angles[zox_tst_mft_turns] = { 90, -90, 180 };
int zox_tst_mft_turn = 0;
byte zox_tst_mft_stage = 0;     // 0 settling, 1 turning
double zox_tst_mft_time = 0;
float zox_tst_mft_done[zox_tst_mft_turns], zox_tst_mft_over[zox_tst_mft_turns];

// the signed angle from where it faces to where the camera turns to (its Euler yaw: the camera eases there, so the
//  turn measured is the camera's and the character's together, as a player sees it), degrees, flat
static inline float zox_tst_mft_off(ecs* world, entity character, entity camera) {
    const float3 f = float4_rotate_float3(zox_getv(character, Rotation3D), (float3) { 0, 0, is_camera_positive_z ? 1 : -1 });
    const float yaw = zox_getv(camera, Euler).y;     // orbit.c: the camera behind at (sin, cos) yaw, looking the other way
    return remainderf(atan2f(-sinf(yaw), -cosf(yaw)) - atan2f(f.x, f.z), 2 * M_PI) * 180 / M_PI;
}

byte zox_tst_move_feel_turn_ready(ecs* world) {
    player_turn_instant = 0;
    if (!zox_tst_zeland_boot_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, zox_players[0], CharacterLink);
    const entity camera = zox_get_link(world, zox_players[0], CameraLink);
    if (!zox_valid(character) || !zox_getv(character, Grounded)) {
        zox_tst_waiting = "the player on the ground";
        return 0;
    }
    if (zox_tst_mft_turn == zox_tst_mft_turns) {
        zox_delta_time_override_count = 0;
        zox_delta_time_override = 0;
        zox_tst_mf_keys(world, 0, 0);
        return 1;
    }
    zox_delta_time_override = zox_tst_mf_dt;
    zox_delta_time_override_count = 2;
    zox_tst_mft_time += zox_tst_mf_dt;
    const int n = zox_tst_mft_turn;
    zox_tst_progress(n * 1000 + zox_tst_mft_stage * 100 + (int) (zox_tst_mft_time * 10));
    if (zox_tst_mft_stage == 0) {
        // facing the camera's way, still: then the camera turns and W is held
        zox_tst_waiting = "facing the camera's way, still";
        zox_tst_mf_keys(world, 0, n == 0);  // the first: W walks it round to face it first
        if (zox_tst_mft_time > 1 && fabsf(zox_tst_mft_off(world, character, camera)) < 1 &&
            fabsf(zox_getv(character, Omega3D).y) < 0.05f) {
            zox_tst_mf_keys(world, 0, 0);
            float3 euler = zox_getv(camera, Euler);
            euler.y += zox_tst_mft_angles[n] * M_PI / 180;
            zox_setv(camera, Euler, euler);
            zox_tst_mft_stage = 1;
            zox_tst_mft_time = 0;
            zox_tst_mft_done[n] = -1;
            zox_tst_mft_over[n] = 0;
        }
        return 0;
    }
    zox_tst_waiting = "it to turn";
    zox_tst_mf_keys(world, 0, 1);
    const float off = zox_tst_mft_off(world, character, camera);
    if (zox_tst_mft_done[n] < 0 && fabsf(off) < 5) {
        zox_tst_mft_done[n] = zox_tst_mft_time;
    }
    // past it: its off the other way than it turned
    const float turned = zox_tst_mft_angles[n] > 0 ? 1 : -1;
    if (zox_tst_mft_done[n] >= 0 && off * turned < 0) {
        zox_tst_mft_over[n] = fmaxf(zox_tst_mft_over[n], fabsf(off));
    }
    if (zox_tst_mft_time > 1.5) {
        zox_tst_mf_keys(world, 0, 0);
        zox_tst_mft_turn++;
        zox_tst_mft_stage = 0;
        zox_tst_mft_time = 0;
    }
    return 0;
}

void zox_tst_move_feel_turn_check(ecs* world) {
    (void) world;
    for (int n = 0; n < zox_tst_mft_turns; n++) {
        zox_log("[tst] [metric] move_feel_turn [%.0f]: faced it in [%.3f]s, swung past [%.1f] degrees", zox_tst_mft_angles[n],
            zox_tst_mft_done[n], zox_tst_mft_over[n]);
        zox_tst_check(zox_tst_mft_done[n] >= 0 && zox_tst_mft_done[n] < 0.5f, "a [%.0f] turn took [%.3f]s", zox_tst_mft_angles[n], zox_tst_mft_done[n]);
        zox_tst_check(zox_tst_mft_over[n] <= 10, "a [%.0f] turn swung [%.1f] degrees past", zox_tst_mft_angles[n], zox_tst_mft_over[n]);
    }
}

void zox_tsts_move_feel_turn(ecs* world) {
    (void) world;
    zox_tst_add("move_feel_turn", zox_tst_move_feel_turn_ready, zox_tst_move_feel_turn_check, 120);
}
