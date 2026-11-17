// The right arm in normal play, with ik: a guard while it's raised, a punch as a motion (not a straight arm)
//  - a punch (SwingStart, SwingSpeed: skills/sys/melee.c, items) over its swing time: the hand pulls back (the
//    elbow bent), drives out to nearly the arm's full reach where the head looks, in towards the body's middle,
//    then recovers to the guard
//  - raised (RaiseShoulder: an item or skill held) the hand rests ahead in a guard, the elbow relaxed, not locked
//  - the weight eases on and off, so the arm never snaps; lowered and not punching it returns to its swing
//  - the head's facing is +z, a first person camera on it faces -z
//  - a held shoulder (IKHeld, XR hands) is left be: the real hand moves it
//  - written in place, not deferred, so the solve (arm_ik.c) reads it this frame
#define ik_punch_guard 0.62f        // of the arm's reach, the hand's rest raised
#define ik_punch_pull 0.45f         // pulled back before it strikes
#define ik_punch_strike 0.97f       // the punch's reach
#define ik_punch_inwards 0.18f      // the strike leans in to the body's middle, of its reach
#define ik_punch_drop 0.12f         // the guard rests a little low, of its reach
#define ik_punch_weight_rate 12.0f  // how fast the weight eases on and off

static inline float ik_smoothstep(float a, float b, float x) {
    float t = (x - a) / (b - a);
    t = t < 0 ? 0 : (t > 1 ? 1 : t);
    return t * t * (3 - 2 * t);
}

// the hand's distance out over the punch, 0 to 1 through it: pull back, strike, recover to the guard
static inline float ik_punch_distance(float p) {
    if (p < 0.3f) {
        return ik_punch_guard + (ik_punch_pull - ik_punch_guard) * ik_smoothstep(0, 0.3f, p);
    }
    if (p < 0.5f) {
        return ik_punch_pull + (ik_punch_strike - ik_punch_pull) * ik_smoothstep(0.3f, 0.5f, p);
    }
    return ik_punch_strike + (ik_punch_guard - ik_punch_strike) * ik_smoothstep(0.55f, 1, p);
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
        const entity camera = zox_get_child_by_id(world, head, zox_id(Camera));
        if (zox_valid(camera)) {
            head = camera;
            facing = float3_backward;
            flip = -1;
        }
        const float4 head_rotation = zox_getv(head, Rotation3D);
        const float3 look = float4_rotate_float3(head_rotation, facing);
        const float3 from = zox_getv(shoulder, Position3D);
        const float3 elbow = zox_getv(arm.bones[arm.length - 2], Position3D);
        const float reach = float3_distance(from, elbow) +
            float3_distance(elbow, zox_getv(arm.bones[arm.length - 1], Position3D));
        const float p = punching ? (float) (passed / speed->value) : 1;
        const float out = punching ? ik_punch_distance(p) : ik_punch_guard;
        // in to the body's middle (the right shoulder's on -x, in is +x) at the strike, low at the guard
        const float strike = punching ? ik_smoothstep(0.3f, 0.5f, p) * (1 - ik_smoothstep(0.55f, 1, p)) : 0;
        const float in_sign = (zox_getv(shoulder, LocalPosition3D).x < 0 ? 1 : -1) * flip;
        const float3 side = float4_rotate_float3(head_rotation, (float3) { in_sign, 0, 0 });
        const float3 down = float3_down;
        float3 target = float3_add(from, float3_scale(look, out * reach));
        target = float3_add(target, float3_scale(side, ik_punch_inwards * strike * reach));
        target = float3_add(target, float3_scale(down, ik_punch_drop * (1 - strike) * reach));
        zox_getm(shoulder, IKTarget)->value = target;
        weight->value += (1 - weight->value) * weight_t;
    }
    zox_sys_on_end();
} zoxd_system(punch_ik_system);
