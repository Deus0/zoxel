// The right arm in normal play, with ik: a guard while it's raised, a swing (a punch, a place, eating) as a motion
//  - a swing (SwingStart, SwingSpeed, SwingKind: bones/fun/swing.c) over its swing time, posed by its kind's entry in
//    swings (dat/swings.c): the hand back, then out to land at its hit, held, recovered to the guard; the body turns
//    with it (SpineTwist, sys/spine.c)
//  - raised (RaiseShoulder: an item or skill held) the hand rests at the guard: ahead where the head looks, a little up
//    and in, the elbow relaxed, not locked; a first person camera sees it (held low by the chest it was out of view)
//  - the weight eases on and off, so the arm never snaps; lowered and not swinging it returns to its swing
//  - the head's facing is +z, a first person camera on it faces -z
//  - the off hand (the left, OffShoulderBoneLink) in a punch: up at the guard too, out ahead as the right winds up,
//    pulled back to the chin as it lands (the recoil, swing_off), held up a moment after (swing_off_linger) so it
//    doesn't drop between punches
//  - a held shoulder (IKHeld, XR hands) is left be: the real hand moves it
//  - written in place, not deferred, so the solve (arm_ik.c) reads it this frame; after physics (zoxp_trails), from
//    where the body is now
#define ik_swing_weight_rate 12.0f  // how fast the weight eases on and off
#define swing_off_linger 0.6        // seconds the off hand's guard stays up after its swing

static inline void swing_weight_ease(IKWeight* weight, float to, float t) {
    weight->value += (to - weight->value) * t;
    if (!to && weight->value < 0.01f) {
        weight->value = 0;
    }
}

// a shoulder's ik, if a swing can move it (not held by XR hands)
static inline IKWeight* swing_weight_of(ecs* world, entity shoulder) {
    return zox_valid(shoulder) && zox_has(shoulder, IKWeight) && !zox_has(shoulder, IKHeld) ?
        zox_getm(shoulder, IKWeight) : NULL;
}

// an arm's hand to a pose: from its shoulder (as the body is now), along the look, in to the body's middle, up
//  - looking up, posed as looking level then turned up about the eyes, not the shoulder, and no further from the
//    shoulder than the pose is level (in along its line from the eyes): turned about the shoulder its up went back with the head's, the hand over and
//    behind the head, out of view, the arm locked straight (tst guard_pitch)
static inline byte swing_arm_target(ecs* world, const skeleton_now* now, entity shoulder, float4 head_rotation,
    float3 look, float flip, swing_pose pose, float3 eye)
{
    ik_arm arm;
    if (!ik_arm_find(world, shoulder, &arm)) {
        return 0;
    }
    const float3 from = skeleton_now_point(now, zox_getv(shoulder, Position3D));
    // the arm's length from its bones as the hierarchy left them, all last frame's: from the shoulder carried to
    //  now to an elbow not carried it read short by however far the body moved or turned since, the guard
    //  (a share of the reach) pulled in and out turning and walking (choppy, tst guard_steady)
    const float3 elbow = zox_getv(arm.bones[arm.length - 2], Position3D);
    const float reach = float3_distance(zox_getv(shoulder, Position3D), elbow) +
        float3_distance(elbow, zox_getv(arm.bones[arm.length - 1], Position3D));
    // the right shoulder's on -x: in is +x
    const float side_sign = zox_getv(shoulder, LocalPosition3D).x < 0 ? 1 : -1;
    const float3 level = float3_normalize((float3) { look.x, 0, look.z });
    const byte looking_up = look.y > 0.001f && (look.x || look.z);
    const float4 lift = looking_up ? quaternion_from_to(level, look) : quaternion_identity;
    const float4 posed = looking_up ? quaternion_rotate(quaternion_inverse(lift), head_rotation) : head_rotation;
    const float3 side = float4_rotate_float3(posed, (float3) { side_sign * flip, 0, 0 });
    const float3 up = float4_rotate_float3(posed, float3_up);
    float3 target = float3_add(from, float3_scale(looking_up ? level : look, pose.out * reach));
    target = float3_add(target, float3_scale(side, pose.in * reach));
    target = float3_add(target, float3_scale(up, pose.up * reach));
    if (looking_up) {
        target = float3_add(eye, float4_rotate_float3(lift, float3_sub(target, eye)));
        // past it: in towards the eyes, so it keeps its place in view
        const float most = sqrtf(pose.out * pose.out + pose.in * pose.in + pose.up * pose.up) * reach;
        const float3 d = float3_sub(target, eye);
        const float3 e = float3_sub(eye, from);
        const float dd = float3_dot(d, d), ed = float3_dot(e, d);
        const float discriminant = ed * ed - dd * (float3_dot(e, e) - most * most);
        if (float3_distance(target, from) > most && dd > 0 && discriminant >= 0) {
            target = float3_add(eye, float3_scale(d, (-ed + sqrtf(discriminant)) / dd));
        }
    }
    zox_getm(shoulder, IKTarget)->value = target;
    return 1;
}

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
        IKWeight* weight = swing_weight_of(world, zox_get_link(world, e, ShoulderBoneLink));
        if (!weight) {
            continue;
        }
        const entity shoulder = zox_get_link(world, e, ShoulderBoneLink);
        const entity off_shoulder = zox_get_link(world, e, OffShoulderBoneLink);
        IKWeight* off_weight = swing_weight_of(world, off_shoulder);
        const double passed = zox_current_time - start->value;
        const byte known = start->value > 0 && speed->value > 0 && passed >= 0 && kind->value < zox_swing_kinds;
        const byte swinging = known && passed < speed->value;
        const byte off_active = off_weight && known && swings[kind->value].has_off && passed < speed->value + swing_off_linger;
        // the right arm holds its guard with the left's linger: both fists up after a punch
        const byte active = swinging || raise->value || off_active;
        if (off_weight && !off_active) {
            swing_weight_ease(off_weight, 0, weight_t);
        }
        if (!active) {
            twist->value = 0;
            swing_weight_ease(weight, 0, weight_t);
            continue;
        }
        entity head = zox_get_link(world, e, HeadBoneLink);
        if (!zox_valid(head)) {
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
        const float3 eye = skeleton_now_point(&now, zox_getv(zox_valid(camera) ? camera : head, Position3D));
        const float p = (float) (passed / speed->value);
        const swing_pose pose = swinging ? swing_pose_at(&swings[kind->value], p) : swing_guard;
        if (!swing_arm_target(world, &now, shoulder, head_rotation, look, flip, pose, eye)) {
            continue;
        }
        const float side_sign = zox_getv(shoulder, LocalPosition3D).x < 0 ? 1 : -1;
        twist->value = pose.twist * side_sign * weight->value;
        swing_weight_ease(weight, 1, weight_t);
        if (off_active) {
            const swing_data off = swing_off(&swings[kind->value]);
            if (swing_arm_target(world, &now, off_shoulder, head_rotation, look, flip, swing_pose_at(&off, p > 1 ? 1 : p), eye)) {
                swing_weight_ease(off_weight, 1, weight_t);
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(swing_ik_system);
