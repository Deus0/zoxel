extern void add_debug_cube(ecs*, entity, float3);
const float3 initial_bone_size = { 0.03f, 0.03f, 0.03f };

entity spawn_prefab_bone(ecs *world) {
    zox_prefab();
    zox_prefab_name("bone");
    zox_add(e, Bone);
    // Transforms
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, float4_identity);
    zox_setv(e, Scale1, 1); // initial_bone_size | 0.5
    zox_setv(e, TransformMatrix, float4x4_identity);
    zox_setv(e, LocalPosition3D, float3_zero);
    zox_setv(e, LocalRotation3D, quaternion_identity);
    zox_setv(e, BonePosition, float3_zero);
    zox_setv(e, BoneSize, float3_zero);
    zox_setv(e, SkeletonLink, 0);
    return e;
}

entity spawn_bone(
    ecs *world,
    entity prefab,
    entity skeleton,
    float3 skeleton_position,
    float3 local_position,
    float3 size)
{
    zox_instance(prefab);
    zox_setv(e, SkeletonLink, skeleton);
    zox_setv(e, LocalPosition3D, local_position);
    zox_setv(e, BonePosition, skeleton_position);
    zox_setv(e, BoneSize, size);
    return e;
}

entity spawn_body_bone(
    ecs* world,
    entity prefab,
    entity skeleton,
    entity parent,
    float3 position,
    float3 local_position,
    float3 size)
{
    // NOTE: New entity so we need to just set rather than grab
    entity e2 = spawn_bone(
        world,
        prefab,
        skeleton,
        position,
        local_position,
        size);
    zox_set_parent(world, e2, parent);
    return e2;
}
