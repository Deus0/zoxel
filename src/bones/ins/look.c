// turns a character's head to look at a target (its head, if it has one), 0 looks ahead again
//  - i.e. both speakers in a dialogue, the player's camera turns with its head
void look_at(ecs* world, entity character, entity target) {
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    if (zox_valid(head)) {
        zox_setv(head, LookTarget, target);
    }
}

// a bone's part's middle as it's posed now (the bone sits at its joint, BonePosition)
float3 bone_centre_now(ecs* world, entity bone) {
    const float3 from_pivot = float3_sub(zox_getv(bone, BoneCentre), zox_getv(bone, BonePosition));
    return float3_add(zox_getv(bone, Position3D), float4_rotate_float3(zox_getv(bone, Rotation3D), from_pivot));
}

// where a character's eyes are, its head's middle, else its position
float3 look_point(ecs* world, entity e) {
    const entity skeleton = get_skeleton(world, e);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    return zox_valid(head) ? bone_centre_now(world, head) : zox_getv(e, Position3D);
}

// who a character's head is looking at (a speaker, in a dialogue), else 0
entity looking_at(ecs* world, entity character) {
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    return zox_valid(head) ? zox_getv(head, LookTarget) : 0;
}
