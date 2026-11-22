// tst: arm_reach, the player's right arm reaches a point with ik (bones/ik) using the skeleton's own bones
//  - holds the right shoulder (IKHeld, as XR hands do) on a point ahead of and below it, within its reach
//  - its hand bone lands on the point, its elbow bends (the forearm turned)
//  - turned as the target rotation (IKTargetRotation, an XR controller's): the hand's fingers along its forward, its
//    thumb up, its palm in (the anatomical rest: palm +z, the right thumb -x), rolled about it the forearm takes half
//    the twist (a wrist turning)
//  - run: bash bsh/tst.sh arm_reach
double zox_tst_arm_reach_start = 0;
float3 zox_tst_arm_reach_target;
#define zox_tst_arm_reach_roll 1.2f

static entity zox_tst_arm_reach_hand(ecs* world, entity shoulder) {
    ik_arm arm;
    return ik_arm_find(world, shoulder, &arm) ? arm.bones[arm.length - 1] : 0;
}

byte zox_tst_arm_reach_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity skeleton = zox_get_link(world, zox_tst_player_character(world), SkeletonLink);
    const entity shoulder = zox_valid(skeleton) ? zox_get_link(world, skeleton, ShoulderBoneLink) : 0;
    const entity hand = zox_valid(shoulder) ? zox_tst_arm_reach_hand(world, shoulder) : 0;
    if (!zox_valid(hand)) {
        zox_tst_waiting = "the player's right arm";
        return 0;
    }
    if (!zox_tst_arm_reach_start) {
        zox_tst_arm_reach_start = zox_current_time;
    }
    // ahead and down, by the body's facing, at 60% of the arm's length
    //  (by the bones' lengths, a bent arm's shoulder to hand would shrink it each frame)
    const float3 from = zox_getv(shoulder, Position3D);
    ik_arm arm;
    ik_arm_find(world, shoulder, &arm);
    const float3 elbow = zox_getv(arm.bones[arm.length - 2], Position3D);
    const float reach = float3_distance(from, elbow) + float3_distance(elbow, zox_getv(hand, Position3D));
    const float4 facing = zox_getv(skeleton, Rotation3D);
    const float3 direction = float3_normalize(float4_rotate_float3(facing, (float3) { 0, -0.5f, 1 }));
    if (zox_current_time - zox_tst_arm_reach_start < 0.5) {
        zox_tst_arm_reach_target = float3_add(from, float3_scale(direction, reach * 0.6f));
    }
    if (!zox_has(shoulder, IKHeld)) {
        zox_add(shoulder, IKHeld);
    }
    // rolled about its forward, as a wrist turned: the forearm takes half the twist
    zox_setv(shoulder, IKTargetRotation, quaternion_rotate(facing, quaternion_from_axis_angle(float3_backward, zox_tst_arm_reach_roll)));
    zox_getm(shoulder, IKTarget)->value = zox_tst_arm_reach_target;
    zox_getm(shoulder, IKWeight)->value = 1;
    zox_tst_waiting = "the arm to reach";
    return zox_current_time - zox_tst_arm_reach_start > 2;
}

void zox_tst_arm_reach_check(ecs* world) {
    const entity skeleton = zox_get_link(world, zox_tst_player_character(world), SkeletonLink);
    const entity shoulder = zox_get_link(world, skeleton, ShoulderBoneLink);
    ik_arm arm;
    zox_tst_check(ik_arm_find(world, shoulder, &arm), "the right arm has no shoulder, elbow and hand");
    if (!ik_arm_find(world, shoulder, &arm)) {
        return;
    }
    const entity hand = arm.bones[arm.length - 1];
    const entity elbow = arm.bones[arm.length - 2];
    const float reach = float3_distance(zox_getv(shoulder, Position3D), zox_getv(elbow, Position3D)) +
        float3_distance(zox_getv(elbow, Position3D), zox_getv(hand, Position3D));
    const float miss = float3_distance(zox_getv(hand, Position3D), zox_tst_arm_reach_target);
    const float4 bend = zox_getv(elbow, LocalRotation3D);
    zox_log("[tst] arm_reach: hand misses by [%.4f] of reach [%.4f], elbow w [%.3f]", miss, reach, bend.w);
    zox_tst_check(miss < reach * 0.05f, "the hand missed its target by [%f] (reach [%f])", miss, reach);
    zox_tst_check(fabsf(bend.w) < 0.999f, "the elbow didn't bend");
    const float3 pointing = float4_rotate_float3(zox_getv(hand, Rotation3D), float3_down);
    const float3 wanted = float4_rotate_float3(zox_getv(shoulder, IKTargetRotation), float3_backward);
    const float turn = float3_dot(pointing, wanted);
    zox_log("[tst] arm_reach: hand points along its target [%.4f]", turn);
    zox_tst_check(turn > 0.99f, "the hand didn't turn to its target [%f]", turn);
    // its thumb up the target's up, as a held controller's (it was upside down): the right hand rests in the
    //  anatomical position, its thumb its -x, its palm +z (bones/ik ik_hand_rest_to_target)
    const float3 thumb = float4_rotate_float3(zox_getv(hand, Rotation3D), (float3) { -1, 0, 0 });
    const float3 up = float4_rotate_float3(zox_getv(shoulder, IKTargetRotation), float3_up);
    const float roll = float3_dot(thumb, up);
    zox_log("[tst] arm_reach: hand's thumb along its target's up [%.4f]", roll);
    zox_tst_check(roll > 0.99f, "the hand is rolled off its target [%f] (upside down at -1)", roll);
    // its palm in, the target's left (a right hand's grip, not a left's mirror)
    const float3 palm = float4_rotate_float3(zox_getv(hand, Rotation3D), (float3) { 0, 0, 1 });
    const float3 left = float4_rotate_float3(zox_getv(shoulder, IKTargetRotation), float3_left);
    zox_log("[tst] arm_reach: hand's palm along its target's left [%.4f]", float3_dot(palm, left));
    zox_tst_check(float3_dot(palm, left) > 0.99f, "the hand's palm isn't in [%f]", float3_dot(palm, left));
    // the wrist's turn shared: the forearm's twist about its line, and the hand's on it, each about half
    const float3 line = float3_normalize(zox_getv(hand, LocalPosition3D));
    const float4 forearm_twist = quaternion_twist(zox_getv(elbow, LocalRotation3D), line);
    const float4 hand_twist = quaternion_twist(zox_getv(hand, LocalRotation3D), line);
    const float forearm_angle = 2 * acosf(float_min(1, fabsf(forearm_twist.w)));
    const float hand_angle = 2 * acosf(float_min(1, fabsf(hand_twist.w)));
    zox_log("[tst] arm_reach: the wrist's twist, the forearm [%.3f] the hand [%.3f] radians", forearm_angle, hand_angle);
    zox_tst_check(forearm_angle > 0.1f && fabsf(forearm_angle - hand_angle) < 0.25f * float_max(forearm_angle, hand_angle),
        "the forearm didn't share the wrist's twist [%f] [%f]", forearm_angle, hand_angle);
}

void zox_tsts_arm_reach(ecs* world) {
    zox_tst_add("arm_reach", zox_tst_arm_reach_ready, zox_tst_arm_reach_check, 120);
}
