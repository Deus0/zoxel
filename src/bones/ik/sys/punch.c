// The right arm in normal play, with ik: a guard while it's raised, a punch or a place as a motion (not a straight arm)
//  - a swing (SwingStart, SwingSpeed, SwingKind: bones/fun/swing.c from an action's warmup) over its swing time, the
//    hit halfway (the warmup's end): the hand goes back, then out to land as it hits, holds a moment, then recovers
//    to the guard
//  - a punch: the fist winds back low by the ribs (the elbow bent) as the body turns away, then snaps out to the arm's
//    full reach where the head looks and in to the body's middle, the body turning into it (SpineTwist, sys/spine.c)
//  - a place: the block lifted up by the head, then pressed out and down where the head looks, a little turn
//  - eating: drawn in by the chest, then up to the mouth, held there a while
//  - raised (RaiseShoulder: an item or skill held) the hand rests ahead where the head looks, a little up and in, the
//    elbow relaxed, not locked; a first person camera sees it (held low by the chest it was out of view: the arm
//    looked gone)
//  - the weight eases on and off, so the arm never snaps; lowered and not swinging it returns to its swing
//  - the head's facing is +z, a first person camera on it faces -z
//  - a held shoulder (IKHeld, XR hands) is left be: the real hand moves it
//  - written in place, not deferred, so the solve (arm_ik.c) reads it this frame; after physics (zoxp_trails), from
//    where the body is now
#define ik_punch_weight_rate 12.0f  // how fast the weight eases on and off
#define ik_swing_hit 0.5f           // through the swing: landed (the warmup's end, the action hits)

// the hand's place, of the arm's reach: out along the look, in to the body's middle, up; and the body's turn
//  (radians, its arm's side forward)
typedef struct {
    float out;
    float in;
    float up;
    float twist;
} ik_swing_pose;

// a swing: back, then out to land at the hit, held, back to the guard
typedef struct {
    ik_swing_pose back;
    ik_swing_pose out;
    float back_end;     // through the swing: gone back, going out
    float held_end;     // held out, the recovery begins
    byte snap;          // out fast then settling (a punch), else eased (a place)
} ik_swing;

static const ik_swing_pose ik_punch_guard = { 0.85f, 0.12f, 0.4f, 0 };   // raised: out where it's seen in first person
static const ik_swing ik_swing_punch = {
    { 0.45f, -0.04f, 0.05f, -0.35f },  // the fist pulled back low by the ribs, the body turned away
    { 0.99f, 0.18f, 0, 0.3f },         // the arm straight, in to the middle, the body turned into it
    0.38f, 0.62f, 1 };
static const ik_swing ik_swing_place = {
    { 0.55f, 0.02f, 0.55f, -0.08f },   // the block lifted up by the head
    { 0.95f, 0.06f, 0.05f, 0.15f },    // pressed out and down where the head looks (below it, the shoulder is)
    0.3f, 0.66f, 0 };
static const ik_swing ik_swing_eat = {
    { 0.7f, 0.2f, -0.05f, 0 },          // drawn in low by the chest
    { 0.55f, 0.42f, 0.3f, -0.05f },     // at the mouth, under the eyes, in to the middle
    0.2f, 0.82f, 0 };

static inline float ik_smoothstep(float a, float b, float x) {
    float t = (x - a) / (b - a);
    t = t < 0 ? 0 : (t > 1 ? 1 : t);
    return t * t * (3 - 2 * t);
}

// fast then settling: the strike's snap
static inline float ik_ease_out(float a, float b, float x) {
    float t = 1 - (x - a) / (b - a);
    t = t < 0 ? 0 : (t > 1 ? 1 : t);
    return 1 - t * t * t;
}

static inline ik_swing_pose ik_swing_pose_lerp(ik_swing_pose a, ik_swing_pose b, float t) {
    return (ik_swing_pose) { a.out + (b.out - a.out) * t, a.in + (b.in - a.in) * t, a.up + (b.up - a.up) * t,
        a.twist + (b.twist - a.twist) * t };
}

// the hand's pose over a swing, 0 to 1 through it: back, out, held, recovered to the guard
static inline ik_swing_pose ik_swing_pose_at(const ik_swing* swing, float p) {
    if (p < swing->back_end) {
        return ik_swing_pose_lerp(ik_punch_guard, swing->back, ik_smoothstep(0, swing->back_end, p));
    }
    if (p < ik_swing_hit) {
        const float t = swing->snap ? ik_ease_out(swing->back_end, ik_swing_hit, p) : ik_smoothstep(swing->back_end, ik_swing_hit, p);
        return ik_swing_pose_lerp(swing->back, swing->out, t);
    }
    if (p < swing->held_end) {
        return swing->out;
    }
    return ik_swing_pose_lerp(swing->out, ik_punch_guard, ik_smoothstep(swing->held_end, 1, p));
}

void punch_ik_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const float delta = zox_delta_time < 0.1f ? zox_delta_time : 0.1f;
    const float weight_t = 1.0f - expf(-ik_punch_weight_rate * delta);
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
        const byte punching = start->value > 0 && speed->value > 0 && passed >= 0 && passed < speed->value;
        const byte active = punching || raise->value;
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
        const ik_swing* swing = kind->value == zox_swing_place ? &ik_swing_place :
            (kind->value == zox_swing_eat ? &ik_swing_eat : &ik_swing_punch);
        const ik_swing_pose pose = punching ? ik_swing_pose_at(swing, (float) (passed / speed->value)) : ik_punch_guard;
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
} zoxd_system(punch_ik_system);
