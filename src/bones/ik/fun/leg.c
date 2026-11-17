// A leg as the skeleton built it: a thigh (ThighBone), its calf (the knee) and foot, each the one bone under the last
//  (hips -> thigh -> calf -> foot from parts/sys/character.c)
typedef struct {
    entity thigh;
    entity calf;
    entity foot;
} ik_leg;

static inline byte ik_leg_find(ecs* world, entity thigh, ik_leg* leg) {
    leg->thigh = thigh;
    leg->calf = zox_get_child_by_id(world, thigh, zox_id(Bone));
    leg->foot = zox_valid(leg->calf) ? zox_get_child_by_id(world, leg->calf, zox_id(Bone)) : 0;
    return zox_valid(leg->calf) && zox_valid(leg->foot);
}

// its length, thigh to calf to foot, from the bones as the hierarchy left them (all one frame's)
static inline float ik_leg_length(ecs* world, const ik_leg* leg) {
    const float3 calf = zox_getv(leg->calf, Position3D);
    return float3_distance(zox_getv(leg->thigh, Position3D), calf) + float3_distance(calf, zox_getv(leg->foot, Position3D));
}

// where its foot stands with the leg straight, now (the body moved since the hierarchy: skeleton_now)
static inline float3 ik_leg_rest(ecs* world, const ik_leg* leg, const skeleton_now* now) {
    const entity hips = zox_get_parent(world, leg->thigh);
    const float4 hips_rotation = skeleton_now_rotation(now, zox_valid(hips) && zox_has(hips, Rotation3D) ?
        zox_getv(hips, Rotation3D) : quaternion_identity);
    const float3 down = float3_add(zox_getv(leg->calf, LocalPosition3D), zox_getv(leg->foot, LocalPosition3D));
    return float3_add(skeleton_now_point(now, zox_getv(leg->thigh, Position3D)), float4_rotate_float3(hips_rotation, down));
}
