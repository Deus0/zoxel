// tst: arms_in_step, a character's arms swing in step after one was raised (bones/sys/arm_control.c)
//  - walking (WalkState held), raises the player's shoulder 0.7s (as attacking does), lowers it, swings a while
//  - its two shoulders swing mirrored: their swing angles (x) sum near 0
//    (each limb kept its own phase, the raised arm's stopped, after it they swung out of step)
//  - run: bash bsh/tst.sh arms_in_step
byte zox_tst_player_spawn_ready(ecs* world);   // in _.c, after this is included
double zox_tst_arms_start = 0;

byte zox_tst_arms_in_step_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity skeleton = zox_get_link(world, character, SkeletonLink);
    if (!zox_valid(skeleton)) {
        zox_tst_waiting = "the player's skeleton";
        return 0;
    }
    if (!zox_tst_arms_start) {
        zox_tst_arms_start = zox_current_time;
    }
    const double t = zox_current_time - zox_tst_arms_start;
    zox_tst_waiting = "the arms to swing";
    // walking, where the swing is quick and an arm out of step shows
    zox_setv(skeleton, WalkState, 1);
    zox_setv(skeleton, RaiseShoulder, t > 1 && t < 1.7);
    return t > 4;
}

void zox_tst_arms_in_step_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity skeleton = zox_get_link(world, character, SkeletonLink);
    zox_tst_check(zox_valid(skeleton), "no skeleton");
    if (!zox_valid(skeleton)) {
        return;
    }
    float angles[2];
    int count = 0;
    const BoneLinks* bones = zox_get(skeleton, BoneLinks);
    for (int i = 0; i < bones->length && count < 2; i++) {
        if (zox_valid(bones->value[i]) && zox_has(bones->value[i], ShoulderBone)) {
            // its swing about x, from the quaternion (euler angles near 90 degrees come out uneven)
            const float4 q = zox_getv(bones->value[i], LocalRotation3D);
            float angle = 2 * atan2f(q.x, q.w);
            angle -= angle > M_PI ? 2 * M_PI : angle < -M_PI ? -2 * M_PI : 0;
            angles[count++] = angle;
        }
    }
    zox_tst_check(count == 2, "[%i] shoulders, not 2", count);
    if (count == 2) {
        zox_log("[tst] shoulder swings [%.3f] [%.3f], sum [%.3f]", angles[0], angles[1], angles[0] + angles[1]);
        zox_tst_check(fabsf(angles[0] + angles[1]) < 0.05f, "the arms swing out of step [%.3f] [%.3f]", angles[0], angles[1]);
    }
}
