// An arm as the skeleton built it: a shoulder, its ArmBone children down to the HandBone
//  (shoulder -> bicep -> forearm -> hand from parts/sys/character.c, any length works)
#define ik_arm_max 8

typedef struct {
    entity bones[ik_arm_max];   // the shoulder first, the hand last
    byte length;
} ik_arm;

static inline byte ik_arm_find(ecs* world, entity shoulder, ik_arm* arm) {
    arm->length = 0;
    entity e = shoulder;
    while (zox_valid(e) && arm->length < ik_arm_max) {
        arm->bones[arm->length++] = e;
        if (e != shoulder && zox_has(e, HandBone)) {
            return arm->length >= 3;    // a shoulder, an elbow and a hand at least
        }
        entity next = zox_get_child_by_id(world, e, zox_id(HandBone));
        if (!zox_valid(next)) {
            next = zox_get_child_by_id(world, e, zox_id(ArmBone));
        }
        e = next;
    }
    return 0;
}

// a bone's offset from the arm's bone at from, down to the bone at to, in from's space (the bones between at rest)
static inline float3 ik_arm_rest(ecs* world, const ik_arm* arm, byte from, byte to) {
    float3 offset = float3_zero;
    for (byte i = from + 1; i <= to; i++) {
        offset = float3_add(offset, zox_getv(arm->bones[i], LocalPosition3D));
    }
    return offset;
}

// Where the hierarchy put a skeleton's bones (last frame) carried to where its owner is now (moved since by physics):
//  the skeleton's world transform as its owner has it now, over the one its bones were placed under
//  - bones' Position3D and Rotation3D are written by the hierarchy (zoxp_transform_hierarchy), after physics moved
//    the owner: read before it they're a frame behind a moving body (a walking body's hand trailed its target)
typedef struct {
    float3 last_position;
    float4 last_inverse;
    float3 now_position;
    float4 now_rotation;
} skeleton_now;

static inline skeleton_now skeleton_now_get(ecs* world, entity skeleton) {
    skeleton_now s = { float3_zero, quaternion_identity, float3_zero, quaternion_identity };
    if (!zox_valid(skeleton)) {
        return s;
    }
    s.last_position = zox_getv(skeleton, Position3D);
    s.last_inverse = quaternion_inverse(zox_getv(skeleton, Rotation3D));
    s.now_position = s.last_position;
    s.now_rotation = zox_getv(skeleton, Rotation3D);
    const entity owner = zox_get_parent(world, skeleton);
    if (zox_valid(owner) && zox_has(owner, Position3D) && zox_has(owner, Rotation3D)) {
        const float4 owner_rotation = zox_getv(owner, Rotation3D);
        const float3 local = zox_has(skeleton, LocalPosition3D) ? zox_getv(skeleton, LocalPosition3D) : float3_zero;
        s.now_position = float3_add(zox_getv(owner, Position3D), float4_rotate_float3(owner_rotation, local));
        s.now_rotation = zox_has(skeleton, LocalRotation3D) ?
            quaternion_rotate(owner_rotation, zox_getv(skeleton, LocalRotation3D)) : owner_rotation;
    }
    return s;
}

static inline float3 skeleton_now_point(const skeleton_now* s, float3 point) {
    return float3_add(s->now_position, float4_rotate_float3(s->now_rotation,
        float4_rotate_float3(s->last_inverse, float3_sub(point, s->last_position))));
}

static inline float4 skeleton_now_rotation(const skeleton_now* s, float4 rotation) {
    return quaternion_rotate(quaternion_rotate(s->now_rotation, s->last_inverse), rotation);
}
