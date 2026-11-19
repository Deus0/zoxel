// tst: place_motion, placing a block swings the arm as a place, not a punch (bones/ik/sys/punch.c, zox_swing_place)
//  - raised (as holding a block), then a slow place swing (3s): the hand lifts above the guard (the block raised by
//    the head), then presses out and down below the look as it lands, the arm out near its reach
//  - seen from the first person camera (it faces -z): up is the hand's y over its distance ahead
//  - run: bash bsh/tst.sh place_motion
double zox_tst_place_motion_start = 0;
#define zox_tst_place_seconds 3.0   // slow, so slow frames still land in each part
float zox_tst_place_guard_up = 0;
float zox_tst_place_lift_up = -1000;    // the most up, lifting
float zox_tst_place_set_up = 1000;      // the least up, set down
float zox_tst_place_set_out = 0;        // the hand from the shoulder setting it, of the arm's reach
byte zox_tst_place_seen = 0;

// the hand's up in the camera's view (its y over its distance ahead), and its reach out of the arm's
static byte zox_tst_place_sample(ecs* world, entity skeleton, entity shoulder, float* up, float* out) {
    ik_arm arm;
    const entity head = zox_get_link(world, skeleton, HeadBoneLink);
    const entity camera = zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
    if (!zox_valid(camera) || !ik_arm_find(world, shoulder, &arm)) {
        return 0;
    }
    const float3 s = zox_getv(shoulder, Position3D);
    const float3 el = zox_getv(arm.bones[arm.length - 2], Position3D);
    const float3 h = zox_getv(arm.bones[arm.length - 1], Position3D);
    const float3 v = float4_rotate_float3(quaternion_inverse(zox_getv(camera, Rotation3D)), float3_sub(h, zox_getv(camera, Position3D)));
    *up = v.z < -0.001f ? v.y / -v.z : 0;
    *out = float3_distance(s, h) / (float3_distance(s, el) + float3_distance(el, h));
    return 1;
}

byte zox_tst_place_motion_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity skeleton = zox_get_link(world, zox_tst_player_character(world), SkeletonLink);
    const entity shoulder = zox_valid(skeleton) ? zox_get_link(world, skeleton, ShoulderBoneLink) : 0;
    if (!zox_valid(shoulder)) {
        zox_tst_waiting = "the player's right arm";
        return 0;
    }
    if (!zox_tst_place_motion_start) {
        zox_tst_place_motion_start = zox_current_time;
    }
    const double t = zox_current_time - zox_tst_place_motion_start;
    zox_tst_progress(t * 10);
    zox_getm(skeleton, RaiseShoulder)->value = 1;
    float up, out;
    if (t < 1.5) {
        zox_tst_waiting = "the guard";
        if (zox_tst_place_sample(world, skeleton, shoulder, &up, &out)) {
            zox_tst_place_seen = 1;
            zox_tst_place_guard_up = up;
        }
        return 0;
    }
    if (zox_getv(skeleton, SwingStart) < zox_tst_place_motion_start + 1.5) {
        zox_setv(skeleton, SwingStart, zox_current_time);
        zox_setv(skeleton, SwingSpeed, zox_tst_place_seconds);
        zox_setv(skeleton, SwingKind, zox_swing_place);
    }
    const double p = (zox_current_time - zox_getv(skeleton, SwingStart)) / zox_tst_place_seconds;
    zox_tst_waiting = "the place";
    if (!zox_tst_place_sample(world, skeleton, shoulder, &up, &out)) {
        return p > 1.1;
    }
    // over wide windows, not narrow ones: at a slow frame rate frames skip them
    if (p > 0.1 && p < 0.35 && up > zox_tst_place_lift_up) {
        zox_tst_place_lift_up = up;
    }
    if (p > 0.45 && p < 0.7) {
        if (up < zox_tst_place_set_up) {
            zox_tst_place_set_up = up;
        }
        if (out > zox_tst_place_set_out) {
            zox_tst_place_set_out = out;
        }
    }
    return p > 1.1;
}

void zox_tst_place_motion_check(ecs* world) {
    zox_log("[tst] place_motion: up (y over ahead) guard [%.3f] lifted [%.3f] set [%.3f], set out [%.3f] of reach",
        zox_tst_place_guard_up, zox_tst_place_lift_up, zox_tst_place_set_up, zox_tst_place_set_out);
    zox_tst_check(zox_tst_place_seen, "no first person camera to see the place from");
    zox_tst_check(zox_tst_place_lift_up > zox_tst_place_guard_up + 0.1f, "the block wasn't lifted above the guard [%f]", zox_tst_place_lift_up);
    zox_tst_check(zox_tst_place_set_up < 0, "the block wasn't set down below the look [%f]", zox_tst_place_set_up);
    zox_tst_check(zox_tst_place_set_out > 0.85f, "the place didn't reach out [%f]", zox_tst_place_set_out);
}

void zox_tsts_place_motion(ecs* world) {
    zox_tst_add("place_motion", zox_tst_place_motion_ready, zox_tst_place_motion_check, 120);
}
