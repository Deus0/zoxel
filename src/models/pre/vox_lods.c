// A single max detail vox model, data only
//  - spawns a VoxLodMesh child per render depth
//  - ModelLods links to those meshes, for instancing
entity spawn_prefab_vox_lods(ecs *world) {
    zox_prefab();
    zox_prefab_name("vox_lods");
    zox_add(e, Model);
    zox_add(e, Vox);
    zox_add(e, VoxLods);
    zox_add(e, Chunk);
    zox_add(e, Chunk3);
    zox_setv(e, ChunkPosition, int3_zero);
    zox_setv(e, ChunkSize, int3_zero);
    zox_prefab_set(e, ChunkNeighbors, { { 0 } } );
    // NOTE: Raycasting reads RenderDepth, this is always max depth
    zox_setv(e, RenderDepth, 0);
    zox_setv(e, MaxRenderDepth, 0);
    zox_setv(e, NodeDepth, 0);
    zox_setv(e, BlockScale, vox_model_scale);
    // allocations
    zox_setv(e, VoxelNode, 0);
    zox_setv(e, VoxelNodeLock, 0);
    zox_setv(e, ColorRGBs, 0);
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
