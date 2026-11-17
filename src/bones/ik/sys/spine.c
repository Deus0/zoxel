// A skeleton's upper body leans (SpineLean) and its hips drop (HipsDrop): XR's body bending and crouching as the
//  player does
//  - the chest (the root bone, the head's parent) turns by the lean about the hips' pivot, which stays put; the hips
//    turn back by it, so they and the legs stay upright (the feet planted, sys/feet.c)
//  - dropped, the chest (and all on it) lowers: the planted feet stay on the ground, the knees bend (leg_ik.c)
//  - the chest's rest place kept (RestPosition3D), its first frame here only notes it
//  - zoxp_trails: after the lean's writers (zoxp_update: XR's room walk), before the hierarchy places the bones
//  - the neck's place leaned and dropped, now: spine_neck_point (XR's eyes ride it)

typedef struct {
    entity chest;
    entity hips;
    entity head;
} spine_bones;

static inline byte spine_bones_find(ecs* world, entity skeleton, spine_bones* bones) {
    bones->head = zox_get_link(world, skeleton, HeadBoneLink);
    bones->chest = zox_valid(bones->head) ? zox_get_parent(world, bones->head) : 0;
    bones->hips = zox_valid(bones->chest) && zox_has(bones->chest, Bone) ? zox_get_child_by_id(world, bones->chest, zox_id(HipsBone)) : 0;
    return zox_valid(bones->hips);
}

// the chest's local place for a lean and drop: turned about the hips' pivot, lowered
static inline float3 spine_chest_position(float3 rest, float3 hips, float4 lean, float drop) {
    return float3_sub(float3_add(rest, float3_sub(hips, float4_rotate_float3(lean, hips))), (float3) { 0, drop, 0 });
}

// the neck (the head bone's pivot) in the skeleton's space for a lean and drop, from the chest's rest
static inline float3 spine_neck_local(ecs* world, const spine_bones* bones, float4 lean, float drop) {
    const float3 rest = zox_has(bones->chest, RestPosition3D) ? zox_getv(bones->chest, RestPosition3D) : zox_getv(bones->chest, LocalPosition3D);
    const float3 chest = spine_chest_position(rest, zox_getv(bones->hips, LocalPosition3D), lean, drop);
    return float3_add(chest, float4_rotate_float3(lean, zox_getv(bones->head, LocalPosition3D)));
}

void spine_lean_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SpineLean);
    zox_sys_in(HipsDrop);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SpineLean, lean);
        zox_sys_i(HipsDrop, drop);
        spine_bones bones;
        if (!spine_bones_find(world, e, &bones)) {
            continue;
        }
        if (!zox_has(bones.chest, RestPosition3D)) {
            zox_setv(bones.chest, RestPosition3D, zox_getv(bones.chest, LocalPosition3D));
            continue;
        }
        const float3 rest = zox_getv(bones.chest, RestPosition3D);
        zox_getm(bones.chest, LocalRotation3D)->value = lean->value;
        zox_getm(bones.chest, LocalPosition3D)->value = spine_chest_position(rest, zox_getv(bones.hips, LocalPosition3D), lean->value, drop->value);
        zox_getm(bones.hips, LocalRotation3D)->value = quaternion_inverse(lean->value);
    }
    zox_sys_on_end();
} zoxd_system(spine_lean_system);
