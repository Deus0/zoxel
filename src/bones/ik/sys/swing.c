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
        // turned up about the eyes, all the way: past the arm's reach the shoulder turns up to it, the hand along
        //  its line from the shoulder, the arm reaching into the sky (pulled in to the eyes instead it came so
        //  close it filled the view's centre looking at the sky; held back from the look the arm hung low, tst guard_pitch)
        target = float3_add(eye, float4_rotate_float3(lift, float3_sub(target, eye)));
        const float most = 0.9f * reach;
        const float3 d = float3_sub(target, from);
        const float length = float3_length(d);
        if (length > most) {
            target = float3_add(from, float3_scale(d, most / length));
        }
    }
    zox_getm(shoulder, IKTarget)->value = target;
    // a fist's hand, a punch's or a place's: its fingers along the arm, its thumb up the view, the palm in (the right
    //  hand's left, ik_hand_rest_to_target), rolled by the pose: the thumb toward the body, the palm down (a punch's
    //  end); not turned as the forearm rolled (looking up its palm faced ahead, a hand up against the sun)
    const float3 view_up = float4_rotate_float3(head_rotation, float3_up);
    const float3 line = float3_sub(target, from);
    if (float3_length(line) > 0.0001f) {
        const float4 fist = ik_turn_to((float3) { 0, 0, -1 }, float3_up, float3_normalize(line), view_up);
        // the thumb rolled toward the body (the palm down), the right's toward the view's left
        const float4 roll = quaternion_from_axis_angle((float3) { 0, 0, 1 }, pose.roll * side_sign);
        zox_set(shoulder, IKTargetRotation, { quaternion_rotate(fist, roll) });
    }
    // its elbow under the arm (down the view), a little out: it bent out to the side reaching up
    zox_set(shoulder, IKPole, { float3_add(float3_scale(view_up, -1), float3_scale(side, -0.3f)) });
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
        // a first person look: the camera's, or the one held while it roams (free roam: the pose stays)
        const entity camera = zox_get_child_by_id(world, head, zox_id(Camera));
        float4 first_person;
        const float4 head_world = head_rotation;
        if (first_person_look(world, e, head, &first_person)) {
            head_rotation = quaternion_rotate(head_rotation, first_person);
            facing = float3_backward;
            flip = -1;
        }
        const float3 look = float4_rotate_float3(head_rotation, facing);
        float3 eye = skeleton_now_point(&now, zox_getv(zox_valid(camera) ? camera : head, Position3D));
        if (!zox_valid(camera) && zox_has(e, LookHeldEye)) {
            // roaming: the eyes it posed from, its camera's place on its head
            eye = float3_add(eye, float4_rotate_float3(head_world, zox_getv(e, LookHeldEye)));
        }
        const float p = (float) (passed / speed->value);
        // its guard: further out holding an item (HoldingItem, actions/sys/hold.c)
        const swing_pose guard = zox_has(e, HoldingItem) ? swing_guard_held : swing_guard;
        const swing_pose pose = swinging ? swing_pose_at(&swings[kind->value], p, guard) : guard;
        if (!swing_arm_target(world, &now, shoulder, head_rotation, look, flip, pose, eye)) {
            continue;
        }
        const float side_sign = zox_getv(shoulder, LocalPosition3D).x < 0 ? 1 : -1;
        twist->value = pose.twist * side_sign * weight->value;
        swing_weight_ease(weight, 1, weight_t);
        if (off_active) {
            const swing_data off = swing_off(&swings[kind->value]);
            if (swing_arm_target(world, &now, off_shoulder, head_rotation, look, flip, swing_pose_at(&off, p > 1 ? 1 : p, swing_guard), eye)) {
                swing_weight_ease(off_weight, 1, weight_t);
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(swing_ik_system);
