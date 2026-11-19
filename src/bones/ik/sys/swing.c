// The right arm in normal play, with ik: a guard while it's raised, a swing (a punch, a place, eating) as a motion
//  - a swing (SwingStart, SwingSpeed, SwingKind: bones/fun/swing.c) over its swing time, posed by its kind's entry in
//    swings (dat/swings.c): the hand back, then out to land at its hit, held, recovered to the guard; the body turns
//    with it (SpineTwist, sys/spine.c)
//  - raised (RaiseShoulder: an item or skill held) the hand rests at the guard: ahead where the head looks, a little up
//    and in, the elbow relaxed, not locked; a first person camera sees it (held low by the chest it was out of view)
//  - the weight eases on and off, so the arm never snaps; lowered and not swinging it returns to its swing
//  - the head's facing is +z, a first person camera on it faces -z
//  - a held shoulder (IKHeld, XR hands) is left be: the real hand moves it
//  - written in place, not deferred, so the solve (arm_ik.c) reads it this frame; after physics (zoxp_trails), from
//    where the body is now
#define ik_swing_weight_rate 12.0f  // how fast the weight eases on and off

void swing_ik_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const float delta = zox_delta_time < 0.1f ? zox_delta_time : 0.1f;
    const float weight_t = 1.0f - expf(-ik_swing_weight_rate * delta);
    zox_sys_begin();
    zox_sys_in(SwingStart);
    zox_sys_in(SwingSpeed);
    zox_sys_in(RaiseShoulder);
    zox_sys_in(SwingKind);
    zox_sys_out(SpineTwist);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SwingStart, start);
        zox_sys_i(SwingSpeed, speed);
        zox_sys_i(RaiseShoulder, raise);
        zox_sys_i(SwingKind, kind);
        zox_sys_o(SpineTwist, twist);
        const entity shoulder = zox_get_link(world, e, ShoulderBoneLink);
        if (!zox_valid(shoulder) || !zox_has(shoulder, IKWeight) || zox_has(shoulder, IKHeld)) {
            continue;
        }
        IKWeight* weight = zox_getm(shoulder, IKWeight);
        const double passed = zox_current_time - start->value;
        const byte swinging = start->value > 0 && speed->value > 0 && passed >= 0 && passed < speed->value &&
            kind->value < zox_swing_kinds;
        const byte active = swinging || raise->value;
        if (!active) {
            twist->value = 0;
            if (weight->value != 0) {
                weight->value += (0 - weight->value) * weight_t;
                if (weight->value < 0.01f) {
                    weight->value = 0;
                }
            }
            continue;
        }
        entity head = zox_get_link(world, e, HeadBoneLink);
        ik_arm arm;
        if (!zox_valid(head) || !ik_arm_find(world, shoulder, &arm)) {
            continue;
        }
        float3 facing = float3_forward;
        float flip = 1;     // a camera faces -z, its x the body's flipped
        // where the body is now (physics moved it after the hierarchy placed the bones), the head and camera
        //  turned as they are this frame (their local turns: the look, a mouse's pitch), not the hierarchy's
        //  last (a frame behind the view: the guard juddered in it turning, tst guard_steady)
        const skeleton_now now = skeleton_now_get(world, e);
        const entity head_parent = zox_get_parent(world, head);
        float4 head_rotation = quaternion_rotate(
            skeleton_now_rotation(&now, zox_valid(head_parent) && zox_has(head_parent, Rotation3D) ?
                zox_getv(head_parent, Rotation3D) : zox_getv(head, Rotation3D)),
            zox_valid(head_parent) && zox_has(head_parent, Rotation3D) ? zox_getv(head, LocalRotation3D) : quaternion_identity);
        const entity camera = zox_get_child_by_id(world, head, zox_id(Camera));
        if (zox_valid(camera)) {
            head_rotation = quaternion_rotate(head_rotation, zox_getv(camera, LocalRotation3D));
            facing = float3_backward;
            flip = -1;
        }
        const float3 look = float4_rotate_float3(head_rotation, facing);
        const float3 from = skeleton_now_point(&now, zox_getv(shoulder, Position3D));
        // the arm's length from its bones as the hierarchy left them, all last frame's: from the shoulder carried to
        //  now to an elbow not carried it read short by however far the body moved or turned since, the guard
        //  (a share of the reach) pulled in and out turning and walking (choppy, tst guard_steady)
        const float3 elbow = zox_getv(arm.bones[arm.length - 2], Position3D);
        const float reach = float3_distance(zox_getv(shoulder, Position3D), elbow) +
            float3_distance(elbow, zox_getv(arm.bones[arm.length - 1], Position3D));
        const swing_pose pose = swinging ? swing_pose_at(&swings[kind->value], (float) (passed / speed->value)) : swing_guard;
        // the right shoulder's on -x: in is +x, its side forward a turn up +y
        const float side_sign = zox_getv(shoulder, LocalPosition3D).x < 0 ? 1 : -1;
        twist->value = pose.twist * side_sign * weight->value;
        const float in_sign = side_sign * flip;
        const float3 side = float4_rotate_float3(head_rotation, (float3) { in_sign, 0, 0 });
        const float3 up = float4_rotate_float3(head_rotation, float3_up);
        float3 target = float3_add(from, float3_scale(look, pose.out * reach));
        target = float3_add(target, float3_scale(side, pose.in * reach));
        target = float3_add(target, float3_scale(up, pose.up * reach));
        zox_getm(shoulder, IKTarget)->value = target;
        weight->value += (1 - weight->value) * weight_t;
    }
    zox_sys_on_end();
} zoxd_system(swing_ik_system);
