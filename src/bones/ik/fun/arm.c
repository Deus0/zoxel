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
