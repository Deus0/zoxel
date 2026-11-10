// A single max detail vox model, data only
//  - spawns a VoxLodMesh child per render depth
//  - ModelLods links to those meshes, for instancing
entity spawn_prefab_vox_lods(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("vox_lods");
    zox_add(e, Model);
    zox_add(e, VoxLods);
    // generation
    zox_setv(e, Seed, 0);
    zox_setv(e, Color, color_white);
    zox_setv(e, VoxType, 0);
    zox_setv(e, GenerateModel, 0);
    zox_prefab_add(e, ModelLods);
    return e;
}

// A mesh per lod of a vox, builds from its parent's voxels
entity spawn_prefab_vox_mesh(ecs *world) {
    zox_prefab();
    zox_prefab_name("vox_mesh");
    zox_add(e, VoxLodMesh);
    zox_setv(e, RenderDepth, 0);
    zox_setv(e, TransformMatrix, float4x4_identity);
    // Rendering
    zox_add(e, Mesh);
    zox_setv(e, Brightness, 1);
    zox_setv(e, RenderDisabled, 0);
    zox_setv(e, MeshIndicies, 0);
    zox_setv(e, MeshRenderCount, 0);
    zox_setv(e, MeshVertices, 0);
    zox_setv(e, MeshGPULink, guint2_zero);
    zox_setv(e, MeshColorRGBs, 0);
    zox_setv(e, ColorsGPULink, 0);
    // Events
    zox_add(e, PreInitialize);
    return e;
}
