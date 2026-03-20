// todo: use iterative instance function: prefab_children
/*void prefab_make_skeleton(ecs *world, const entity e) {
    zox_add_tag(e, Skeleton)
    zox_add_tag(e, SkeletonMesher)
    zox_prefab_set(e, Children, { 0, NULL });
    zox_prefab_add(e, BoneLinks)
    zox_prefab_add(e, BoneIndexes)
    // add_gpu_bone_index(world, e);
    // zox_prefab_add(e, BoneLinks)
    spawn_gpu_bone_index(world, e);
#ifdef zox_paint_skeletons
    zox_add_tag(e, PaintedSkeleton)
#endif
}*/

entity spawn_body_bone(ecs* world, entity p, entity skeleton, BoneLinks* bones, entity parent, Children* children, float3 position, float3 local_position, float3 size) {

    // NOTE: New entity so we need to just set rather than grab
    entity e2 = spawn_bone(world, p, skeleton, position, local_position, size);
    zox_set(e2, ParentLink, { parent });
    add_to_Children(children, e2);
    add_to_BoneLinks(bones, e2);

    return e2;

    /*if (!zox_valid(skeleton)) {
        zox_logw("Skeleton Invalid [spawn_body_bone]");
        return 0;
    }

    if (!zox_valid(parent)) {
        zox_logw("Parent Invalid [spawn_body_bone]");
        return 0;
    }

    if (!zox_has(skeleton, BoneLinks)) {
        zox_logw("Skeleton [%s] has no BoneLinks", zox_get_name(skeleton));
        return 0;
    }*/
}

// will this effect other children though?
entity spawn_skeleton_bones(ecs *world, entity e, BoneLinks* bones, float head_move_y) {
    // prefab_make_skeleton(world, e);

    // todo: apply voxel scale to skeleton spawning
    // float scale = 1 / 64.0f; // player_vox_scale;

    float3 test_bone_size_body = float3_single(0.08f);

    float3 bone_head_position = (float3) { 0, 0.22f, 0 }; //  -0.03f * 0.5f };
    float3 test_bone_head_size = float3_single(0.1f);

    // animating
    float head_delta = randf_range(0.01f, 0.01f + head_move_y);
    float3 delta = (float3) { 0, head_delta, 0 };

    // Children children = (Children) { 0 };
    // BoneLinks bones = (BoneLinks) { 0 };

    // TODO: SkeletonDirty, just fetch bones there into list
    resize_BoneLinks(bones, 0);

    // = Body Bone =
    Children schildren = (Children) { 0 };
    entity bone_body = spawn_body_bone(world, prefab_bone, e, bones, e, &schildren, float3_zero, float3_zero, test_bone_size_body);
    zox_set_ptr(e, Children, schildren);
    Children core_children = (Children) { 0 };

    // = Head Bone =
    entity bone_child = spawn_body_bone(world, prefab_bone, e, bones, bone_body, &core_children, bone_head_position, bone_head_position, test_bone_head_size);
    zox_add_tag(bone_child, HeadBone);

    // Finish Core Children
    zox_set_ptr(bone_body, Children, core_children);

    // head
    // float3 test_bone_position_animating = (float3) { 0, head_delta * 2, 0 };
    zox_add_tag(bone_child, OscillatePosition3D);
    zox_set(bone_child, OscillateStartPosition3D, { float3_add(bone_head_position, delta) });
    zox_set(bone_child, OscillateDeltaPosition3D, { delta });

    /*entity bone_body = spawn_bone(world, prefab_bone, e, float3_zero, float3_zero, test_bone_size_body);
    zox_set(bone_body, ParentLink, { e })
    add_to_Children(&children, bone_body);
    add_to_BoneLinks(&bones, bone_body);*/


    /*Children bone_children = (Children) { 0 };
    entity bone_child = spawn_bone(world, prefab_bone, e, bone_head_position, bone_head_position, test_bone_head_size);
    zox_set(bone_child, ParentLink, { bone_body });*/

    // add_to_Children(&bone_children, bone_child);

    // add_to_BoneLinks(&bones, bone_child);
    // zox_set_ptr(bone_body, Children, bone_children)

    // zox_set_ptr(e, Children, children);
    // zox_set_ptr(e, BoneLinks, bones);

    return bone_body;
}
