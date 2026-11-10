// A skeleton, spawned as a child of its owner, i.e. a character
//  - bones are its children, linked with BoneLinks, HeadBoneLink, HandBoneLink, ShoulderBoneLink
//  - a skeleton_mesh child per render depth, built from its owners vox (VoxLink)
//  - holds animation state: WalkState, RaiseShoulder, SwingStart, SwingSpeed
//  - its transform follows its owner
entity spawn_prefab_skeleton(ecs *world) {
    zox_prefab();
    zox_prefab_name("skeleton");
    zox_add(e, Skeleton);
    zox_setv(e, BoneLinks, 0);
    // Animation
    zox_setv(e, RaiseShoulder, 0);
    zox_setv(e, WalkState, 0);
    zox_setv(e, SwingStart, 0);
    zox_setv(e, SwingSpeed, 0);
    // Transforms
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, float4_identity);
    zox_setv(e, Scale1, 1);
    zox_setv(e, LocalPosition3D, float3_zero);
    zox_setv(e, LocalRotation3D, quaternion_identity);
    // TODO: Add this to prefab in settings
    if (is_paint_skeletons) {
        zox_add(e, PaintedSkeleton);
    }
    return e;
}
