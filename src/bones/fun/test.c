
entity spawn_body_bone(ecs* world, entity prefab, entity skeleton, entity parent, float3 position, float3 local_position, float3 size) {
    // NOTE: New entity so we need to just set rather than grab
    entity e2 = spawn_bone(world, prefab, skeleton, position, local_position, size);
    zox_set_parent(world, e2, parent);
    // zox_set(e2, ParentLink, { parent });
    return e2;
}

// todo: use iterative instance function: prefab_children
/*void prefab_make_skeleton(ecs *world, const entity e) {
    zox_add_tag(e, Skeleton)
    zox_add_tag(e, SkeletonMesher)
    zox_prefab_add(e, BoneLinks)
    zox_prefab_add(e, BoneIndexes)
    // add_gpu_bone_index(world, e);
    // zox_prefab_add(e, BoneLinks)
    spawn_gpu_bone_index(world, e);
#ifdef zox_paint_skeletons
    zox_add_tag(e, PaintedSkeleton)
#endif
}*/

// will this effect other children though?
/*entity spawn_skeleton_head_bone(ecs *world, entity e, entity parent, Children* children, BoneLinks* bones, float head_move_y, float bscale) {
    // prefab_make_skeleton(world, e);

    float3 bone_head_position = (float3) { 0, 0.17f, 0 };
    float3 test_bone_head_size = float3_single(0.071f);

    // add half a voxel to center the bone positions
    zox_log("bscale: %f", bscale);
    bone_head_position = float3_add(bone_head_position, float3_single(bscale / 2));

    // animating
    float head_delta = randf_range(0.01f, 0.01f + head_move_y);
    float3 delta = (float3) { 0, head_delta, 0 };

    // = Head Bone =
    entity bone_child = spawn_body_bone(world, prefab_bone, e, bones, parent, children, bone_head_position, bone_head_position, test_bone_head_size);
    zox_add_tag(bone_child, HeadBone);

    // head
    // float3 test_bone_position_animating = (float3) { 0, head_delta * 2, 0 };
    zox_add_tag(bone_child, OscillatePosition3D);
    zox_set(bone_child, OscillateStartPosition3D, { float3_add(bone_head_position, delta) });
    zox_set(bone_child, OscillateDeltaPosition3D, { delta });

    return bone_child;
}*/
