extern void add_debug_cube(ecs *world, const entity e, const float3 size);
const float3 initial_bone_size = { 0.03f, 0.03f, 0.03f };

entity spawn_prefab_bone(ecs *world) {
    zox_prefab();
    zox_prefab_name("bone");

    zox_add_tag(e, Bone);
    zox_prefab_set(e, SkeletonLink, { 0 });
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, Scale1D, { 1 }); // initial_bone_size | 0.5
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    zox_prefab_set(e, LocalPosition3D, { float3_zero });
    zox_prefab_set(e, LocalRotation3D, { quaternion_identity });
    zox_prefab_set(e, BonePosition, { float3_zero });
    zox_prefab_set(e, BoneSize, { float3_zero });
    zox_prefab_set(e, Children, { 0 });

    return e;
}

entity spawn_bone(ecs *world, entity p, entity skeleton, float3 skeleton_position, const float3 local_position, float3 size) {

    zox_instance(p);

    zox_set(e, SkeletonLink, { skeleton });
    zox_set(e, LocalPosition3D, { local_position });
    zox_set(e, Position3D, { skeleton_position });
    zox_set(e, BonePosition, { skeleton_position }); // actually position within skeleton, local to root bone
    zox_set(e, BoneSize, { size });

    /*if (render_bones) {
        add_debug_cube(world, e, bone_size);
    }*/

    return e;
}
