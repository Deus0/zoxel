// A skinned mesh per render depth of a skeleton character
//  - spawned when its depth is first needed, then kept, like terrain chunk meshes
//  - builds from its parent character's voxels, uses its parent's bones and transform
entity spawn_prefab_skeleton_mesh(ecs *world) {
    zox_prefab();
    zox_prefab_name("skeleton_mesh");
    zox_add(e, SkeletonMesh);
    zox_setv(e, RenderDepth, 0);
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
