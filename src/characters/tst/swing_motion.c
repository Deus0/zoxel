// tst: swing_motion, placing a block and eating swing the arm as themselves, not a punch (bones/ik/sys/punch.c)
//  - raised (as holding a block), then a slow place swing (3s): the hand lifts above the guard (the block raised by
//    the head), then presses out and down below the look as it lands, the arm out near its reach
//  - then a slow eat swing (3s): the hand comes to the mouth (near the head, under the eyes), far closer than the guard
//  - seen from the first person camera (it faces -z): up is the hand's y over its distance ahead
//  - run: bash bsh/tst.sh swing_motion
double zox_tst_swing_motion_start = 0;
#define zox_tst_place_seconds 3.0   // slow, so slow frames still land in each part
float zox_tst_place_guard_up = 0;
float zox_tst_place_lift_up = -1000;    // the most up, lifting
float zox_tst_place_set_up = 1000;      // the least up, set down
float zox_tst_place_set_out = 0;        // the hand from the shoulder setting it, of the arm's reach
byte zox_tst_place_seen = 0;
float zox_tst_eat_guard = 0;        // the hand from the head at the guard, of the arm's reach
float zox_tst_eat_mouth = 1000;     // its least eating
float zox_tst_eat_mouth_up = 1000;  // the hand's up in the view then (under the eyes)
float zox_tst_eat_mouth_ahead = 0;  // and its distance ahead of the camera (in front of the face, not through it)

// the hand from the head, of the arm's reach
static float zox_tst_eat_from_head(ecs* world, entity skeleton, entity shoulder) {
    ik_arm arm;
    const entity head = zox_get_link(world, skeleton, HeadBoneLink);
    if (!zox_valid(head) || !ik_arm_find(world, shoulder, &arm)) {
        return 1000;
    }
    const float3 s = zox_getv(shoulder, Position3D);
    const float3 el = zox_getv(arm.bones[arm.length - 2], Position3D);
    const float3 h = zox_getv(arm.bones[arm.length - 1], Position3D);
    return float3_distance(zox_getv(head, Position3D), h) / (float3_distance(s, el) + float3_distance(el, h));
}

// the hand's up in the camera's view (its y over its distance ahead), and its reach out of the arm's
float3 zox_tst_swing_seen;     // the hand in the camera's space, last sampled

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
    zox_tst_swing_seen = v;
    *up = v.z < -0.001f ? v.y / -v.z : 0;
    *out = float3_distance(s, h) / (float3_distance(s, el) + float3_distance(el, h));
    return 1;
}

byte zox_tst_swing_motion_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity skeleton = zox_get_link(world, zox_tst_player_character(world), SkeletonLink);
    const entity shoulder = zox_valid(skeleton) ? zox_get_link(world, skeleton, ShoulderBoneLink) : 0;
    if (!zox_valid(shoulder)) {
        zox_tst_waiting = "the player's right arm";
        return 0;
    }
    if (!zox_tst_swing_motion_start) {
        zox_tst_swing_motion_start = zox_current_time;
    }
    const double t = zox_current_time - zox_tst_swing_motion_start;
    zox_tst_progress(t * 10);
    zox_getm(skeleton, RaiseShoulder)->value = 1;
    float up, out;
    if (t < 1.5) {
        zox_tst_waiting = "the guard";
        if (zox_tst_place_sample(world, skeleton, shoulder, &up, &out)) {
            zox_tst_place_seen = 1;
            zox_tst_place_guard_up = up;
        }
        zox_tst_eat_guard = zox_tst_eat_from_head(world, skeleton, shoulder);
        return 0;
    }
    if (zox_getv(skeleton, SwingStart) < zox_tst_swing_motion_start + 1.5) {
        zox_setv(skeleton, SwingStart, zox_current_time);
        zox_setv(skeleton, SwingSpeed, zox_tst_place_seconds);
        zox_setv(skeleton, SwingKind, zox_swing_place);
    }
    const double p = (zox_current_time - zox_getv(skeleton, SwingStart)) / zox_tst_place_seconds;
    if (zox_getv(skeleton, SwingKind) == zox_swing_place && p > 1.1) {
        zox_setv(skeleton, SwingStart, zox_current_time);
        zox_setv(skeleton, SwingKind, zox_swing_eat);
        return 0;
    }
    if (zox_getv(skeleton, SwingKind) == zox_swing_eat) {
        zox_tst_waiting = "eating";
        const float from_head = zox_tst_eat_from_head(world, skeleton, shoulder);
        if (p > 0.45 && p < 0.8 && from_head < zox_tst_eat_mouth) {
            zox_tst_eat_mouth = from_head;
            if (zox_tst_place_sample(world, skeleton, shoulder, &up, &out)) {
                zox_tst_eat_mouth_up = up;
                zox_tst_eat_mouth_ahead = -zox_tst_swing_seen.z;
            }
        }
        return p > 1.1;
    }
    zox_tst_waiting = "the place";
    if (!zox_tst_place_sample(world, skeleton, shoulder, &up, &out)) {
        return 0;
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
    return 0;
}

void zox_tst_swing_motion_check(ecs* world) {
    zox_log("[tst] swing_motion: up (y over ahead) guard [%.3f] lifted [%.3f] set [%.3f], set out [%.3f] of reach",
        zox_tst_place_guard_up, zox_tst_place_lift_up, zox_tst_place_set_up, zox_tst_place_set_out);
    zox_tst_check(zox_tst_place_seen, "no first person camera to see the place from");
    zox_tst_check(zox_tst_place_lift_up > zox_tst_place_guard_up + 0.1f, "the block wasn't lifted above the guard [%f]", zox_tst_place_lift_up);
    zox_tst_check(zox_tst_place_set_up < 0, "the block wasn't set down below the look [%f]", zox_tst_place_set_up);
    zox_tst_check(zox_tst_place_set_out > 0.85f, "the place didn't reach out [%f]", zox_tst_place_set_out);
    zox_log("[tst] swing_motion: the hand from the head (of reach) guard [%.3f] eating [%.3f], up in the view [%.3f] ahead [%.3f]",
        zox_tst_eat_guard, zox_tst_eat_mouth, zox_tst_eat_mouth_up, zox_tst_eat_mouth_ahead);
    zox_tst_check(zox_tst_eat_mouth < zox_tst_eat_guard * 0.7f, "the hand didn't come to the mouth [%f of %f]", zox_tst_eat_mouth, zox_tst_eat_guard);
    zox_tst_check(zox_tst_eat_mouth_up < 0, "eating, the hand isn't under the eyes [%f]", zox_tst_eat_mouth_up);
    zox_tst_check(zox_tst_eat_mouth_ahead > 0, "eating, the hand went through the face [%f]", zox_tst_eat_mouth_ahead);
}

void zox_tsts_swing_motion(ecs* world) {
    zox_tst_add("swing_motion", zox_tst_swing_motion_ready, zox_tst_swing_motion_check, 120);
}
