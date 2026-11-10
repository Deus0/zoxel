// A skinned mesh per render depth, a child of a skeleton
//  - spawned when its depth is first needed, then kept, like terrain chunk meshes
//  - builds from its skeletons owner vox (VoxLink), uses its skeletons bones
//  - transform follows its skeleton, RenderDisabled is set with its owner
entity spawn_prefab_skeleton_mesh(ecs *world) {
    zox_prefab();
    zox_prefab_name("skeleton_mesh");
    zox_add(e, SkeletonMesh);
    zox_setv(e, RenderDepth, 0);
    // Transforms
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, float4_identity);
    zox_setv(e, Scale1, 1);
    zox_setv(e, LocalPosition3D, float3_zero);
    zox_setv(e, LocalRotation3D, quaternion_identity);
    zox_setv(e, TransformMatrix, float4x4_identity);
    zox_setv(e, RenderDisabled, 0);
    // Rendering
    zox_add(e, Mesh);
    zox_setv(e, Brightness, 1);
    zox_setv(e, MeshIndicies, 0);
    zox_setv(e, MeshRenderCount, 0);
    zox_setv(e, MeshVertices, 0);
    zox_setv(e, MeshGPULink, guint2_zero);
    zox_setv(e, MeshColorRGBs, 0);
    zox_setv(e, ColorsGPULink, 0);
    // Bones
    zox_setv(e, BoneIndexes, 0);
    zox_setv(e, BoneIndexGPULink, 0);
    // NOTE: Start Building and Disabled, enabled when swapped in
    zox_add(e, Disabled);
    zox_add(e, BuildMesh);
    // Events
    zox_add(e, PreInitialize);
    return e;
}
