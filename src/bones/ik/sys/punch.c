// The right arm in normal play, with ik: a guard while it's raised, a punch as a motion (not a straight arm)
//  - a punch (SwingStart, SwingSpeed: skills/sys/melee_swing.c from the warmup, items) over its swing time, the hit
//    halfway (the warmup's end): the fist winds back low by the ribs (the elbow bent), snaps out to the arm's full
//    reach where the head looks and in to the body's middle as it hits, holds a moment, then recovers to the guard
//  - raised (RaiseShoulder: an item or skill held) the hand rests ahead where the head looks, a little up and in, the
//    elbow relaxed, not locked; a first person camera sees it (held low by the chest it was out of view: the arm
//    looked gone)
//  - the weight eases on and off, so the arm never snaps; lowered and not punching it returns to its swing
//  - the head's facing is +z, a first person camera on it faces -z
//  - a held shoulder (IKHeld, XR hands) is left be: the real hand moves it
//  - written in place, not deferred, so the solve (arm_ik.c) reads it this frame; after physics (zoxp_trails), from
//    where the body is now
#define ik_punch_weight_rate 12.0f  // how fast the weight eases on and off
#define ik_punch_chambered 0.38f    // through the swing: wound back, the strike begins
#define ik_punch_hit 0.5f           // landed (the warmup's end, melee.c hits)
#define ik_punch_held 0.62f         // held out, the recovery begins

// the hand's place, of the arm's reach: out along the look, in to the body's middle, up
typedef struct {
    float out;
    float in;
    float up;
} ik_punch_pose;

static const ik_punch_pose ik_punch_guard = { 0.85f, 0.12f, 0.4f };    // raised: out where it's seen in first person
static const ik_punch_pose ik_punch_wound = { 0.45f, -0.04f, 0.05f };  // the fist pulled back low by the ribs
static const ik_punch_pose ik_punch_strike = { 0.99f, 0.18f, 0 };      // the arm straight, in to the middle

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

static inline ik_punch_pose ik_punch_pose_lerp(ik_punch_pose a, ik_punch_pose b, float t) {
    return (ik_punch_pose) { a.out + (b.out - a.out) * t, a.in + (b.in - a.in) * t, a.up + (b.up - a.up) * t };
}

// the hand's pose over the punch, 0 to 1 through it: wind up, strike, hold, recover to the guard
static inline ik_punch_pose ik_punch_pose_at(float p) {
    if (p < ik_punch_chambered) {
        return ik_punch_pose_lerp(ik_punch_guard, ik_punch_wound, ik_smoothstep(0, ik_punch_chambered, p));
    }
    if (p < ik_punch_hit) {
        return ik_punch_pose_lerp(ik_punch_wound, ik_punch_strike, ik_ease_out(ik_punch_chambered, ik_punch_hit, p));
    }
    if (p < ik_punch_held) {
        return ik_punch_strike;
    }
    return ik_punch_pose_lerp(ik_punch_strike, ik_punch_guard, ik_smoothstep(ik_punch_held, 1, p));
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
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SwingStart, start);
        zox_sys_i(SwingSpeed, speed);
        zox_sys_i(RaiseShoulder, raise);
        const entity shoulder = zox_get_link(world, e, ShoulderBoneLink);
        if (!zox_valid(shoulder) || !zox_has(shoulder, IKWeight) || zox_has(shoulder, IKHeld)) {
            continue;
        }
        IKWeight* weight = zox_getm(shoulder, IKWeight);
        const double passed = zox_current_time - start->value;
        const byte punching = start->value > 0 && speed->value > 0 && passed >= 0 && passed < speed->value;
        const byte active = punching || raise->value;
        if (!active) {
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
        const ik_punch_pose pose = punching ? ik_punch_pose_at((float) (passed / speed->value)) : ik_punch_guard;
        // in to the body's middle: the right shoulder's on -x, in is +x
        const float in_sign = (zox_getv(shoulder, LocalPosition3D).x < 0 ? 1 : -1) * flip;
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
