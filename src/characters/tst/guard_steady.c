// tst: guard_steady, the raised guard stays put in the first person view as the view turns (bones/ik/sys/punch.c)
//  - raised, settled, then the body turns steadily as a mouse look does and walks ahead, the pitch held
//  - the hand in the camera's space stays where it settled (turning and walking the guard is still in view): the
//    arm's reach was measured between bones of two frames (the shoulder carried to now, the elbow not), short by
//    the body's move, so the guard pulled in and out turning and walking (choppy), 0.39 of reach off
//  - run: bash bsh/tst.sh guard_steady
double zox_tst_guard_steady_start = 0;
float3 zox_tst_guard_steady_rest;
float zox_tst_guard_steady_worst = -1;     // the hand from its rest in the camera's space, worst, of the arm's reach
int zox_tst_guard_steady_frames = 0;

static inline byte zox_tst_guard_steady_hand(ecs* world, entity skeleton, float3* seen, float* reach) {
    const entity shoulder = zox_get_link(world, skeleton, ShoulderBoneLink);
    const entity head = zox_get_link(world, skeleton, HeadBoneLink);
    const entity camera = zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
    ik_arm arm;
    if (!zox_valid(camera) || !zox_valid(shoulder) || !ik_arm_find(world, shoulder, &arm)) {
        return 0;
    }
    const float3 elbow = zox_getv(arm.bones[arm.length - 2], Position3D);
    const float3 hand = zox_getv(arm.bones[arm.length - 1], Position3D);
    *reach = float3_distance(zox_getv(shoulder, Position3D), elbow) + float3_distance(elbow, hand);
    *seen = float4_rotate_float3(quaternion_inverse(zox_getv(camera, Rotation3D)), float3_sub(hand, zox_getv(camera, Position3D)));
    return 1;
}

byte zox_tst_guard_steady_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity skeleton = zox_get_link(world, character, SkeletonLink);
    if (!zox_valid(skeleton)) {
        zox_tst_waiting = "the player's skeleton";
        return 0;
    }
    if (!zox_tst_guard_steady_start) {
        zox_tst_guard_steady_start = zox_current_time;
    }
    const double t = zox_current_time - zox_tst_guard_steady_start;
    zox_tst_progress(t * 10);
    zox_getm(skeleton, RaiseShoulder)->value = 1;
    float3 seen;
    float reach;
    if (!zox_tst_guard_steady_hand(world, skeleton, &seen, &reach)) {
        zox_tst_waiting = "the player's arm and camera";
        return 0;
    }
    zox_tst_waiting = "the guard, then turning";
    if (t < 1.5) {
        zox_tst_guard_steady_rest = seen;
        return 0;
    }
    if (zox_tst_guard_steady_frames > 2) {
        const float off = float3_distance(seen, zox_tst_guard_steady_rest) / reach;
        zox_tst_guard_steady_worst = float_max(zox_tst_guard_steady_worst, off);
    }
    // turned as a mouse would (controllers3/sys/rotation.c): the body's yaw, the camera's pitch
    const int f = zox_tst_guard_steady_frames++;
    float3 euler = zox_getv(character, Euler);
    euler.y += 0.05f;
    zox_setv(character, Euler, euler);
    zox_setv(character, Rotation3D, quaternion_from_euler(euler));
    const float3 ahead = float4_rotate_float3(quaternion_from_euler(euler), (float3) { 0, 0, 2 });
    Velocity3D* velocity = zox_getm(character, Velocity3D);
    velocity->value.x = ahead.x;
    velocity->value.z = ahead.z;
    return f > 60;
}

void zox_tst_guard_steady_check(ecs* world) {
    (void) world;
    zox_log("[tst] guard_steady: over [%i] frames turning, the hand off its place in view, worst [%.4f] of reach",
        zox_tst_guard_steady_frames, zox_tst_guard_steady_worst);
    zox_tst_check(zox_tst_guard_steady_worst >= 0, "never measured");
    zox_tst_check(zox_tst_guard_steady_worst < 0.05f, "the guard juddered in view turning [%.3f of reach]", zox_tst_guard_steady_worst);
}

void zox_tsts_guard_steady(ecs* world) {
    zox_tst_add("guard_steady", zox_tst_guard_steady_ready, zox_tst_guard_steady_check, 120);
}
