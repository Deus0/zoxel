// tst: punch_motion, the player's punch bends its arm as a motion (bones/ik/sys/punch.c), not a straight arm
//  - raised (as holding the punch skill), then a punch of 1s: pulled back the elbow bends well, at the strike
//    the hand is out near the arm's full reach, and before it the guard's elbow is relaxed (bent some)
//  - run: bash bsh/tst.sh punch_motion
double zox_tst_punch_motion_start = 0;
float zox_tst_punch_guard_bend = 1;     // the elbow's w: 1 straight, less bent
float zox_tst_punch_pull_bend = 1;
float zox_tst_punch_strike_out = 0;     // the hand from the shoulder, of the arm's reach

static float zox_tst_punch_motion_sample(ecs* world, entity shoulder, float* bend) {
    ik_arm arm;
    if (!ik_arm_find(world, shoulder, &arm)) {
        return 0;
    }
    const entity elbow = arm.bones[arm.length - 2];
    const entity hand = arm.bones[arm.length - 1];
    const float3 s = zox_getv(shoulder, Position3D);
    const float3 el = zox_getv(elbow, Position3D);
    const float3 h = zox_getv(hand, Position3D);
    *bend = fabsf(zox_getv(elbow, LocalRotation3D).w);
    return float3_distance(s, h) / (float3_distance(s, el) + float3_distance(el, h));
}

byte zox_tst_punch_motion_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity skeleton = zox_get_link(world, zox_tst_player_character(world), SkeletonLink);
    const entity shoulder = zox_valid(skeleton) ? zox_get_link(world, skeleton, ShoulderBoneLink) : 0;
    if (!zox_valid(shoulder)) {
        zox_tst_waiting = "the player's right arm";
        return 0;
    }
    if (!zox_tst_punch_motion_start) {
        zox_tst_punch_motion_start = zox_current_time;
    }
    const double t = zox_current_time - zox_tst_punch_motion_start;
    zox_getm(skeleton, RaiseShoulder)->value = 1;
    float bend;
    if (t < 1.5) {
        zox_tst_waiting = "the guard";
        zox_tst_punch_guard_bend = 1;
        zox_tst_punch_motion_sample(world, shoulder, &zox_tst_punch_guard_bend);
        return 0;
    }
    if (zox_getv(skeleton, SwingStart) < zox_tst_punch_motion_start + 1.5) {
        zox_setv(skeleton, SwingStart, zox_current_time);
        zox_setv(skeleton, SwingSpeed, 1);
    }
    const double p = zox_current_time - zox_getv(skeleton, SwingStart);
    const float out = zox_tst_punch_motion_sample(world, shoulder, &bend);
    if (p > 0.15 && p < 0.3 && bend < zox_tst_punch_pull_bend) {
        zox_tst_punch_pull_bend = bend;
    }
    if (p > 0.45 && p < 0.6 && out > zox_tst_punch_strike_out) {
        zox_tst_punch_strike_out = out;
    }
    zox_tst_waiting = "the punch";
    return p > 1.1;
}

void zox_tst_punch_motion_check(ecs* world) {
    zox_log("[tst] punch_motion: guard elbow w [%.3f], pulled back w [%.3f], strike out [%.3f] of reach",
        zox_tst_punch_guard_bend, zox_tst_punch_pull_bend, zox_tst_punch_strike_out);
    zox_tst_check(zox_tst_punch_guard_bend < 0.98f, "the guard's elbow is locked straight [%f]", zox_tst_punch_guard_bend);
    zox_tst_check(zox_tst_punch_pull_bend < 0.85f, "pulled back, the elbow barely bent [%f]", zox_tst_punch_pull_bend);
    zox_tst_check(zox_tst_punch_strike_out > 0.85f, "the strike didn't reach out [%f]", zox_tst_punch_strike_out);
}

void zox_tsts_punch_motion(ecs* world) {
    zox_tst_add("punch_motion", zox_tst_punch_motion_ready, zox_tst_punch_motion_check, 120);
}
