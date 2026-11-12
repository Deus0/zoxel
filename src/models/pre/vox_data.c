// Voxel data only, no transform or rendering
//  - meshes or instance renderers are kept on other entities
entity spawn_prefab_vox_data(ecs *world) {
    zox_prefab();
    zox_prefab_name("vox_data");
    zox_add(e, Vox);
    zox_add(e, Chunk);
    zox_add(e, Chunk3);
    zox_setv(e, ChunkPosition, int3_zero);
    zox_setv(e, ChunkSize, int3_zero);
    zox_set(e, ChunkNeighbors, { { 0 } } );
    // NOTE: Raycasting reads RenderDepth, this is always max depth
    zox_setv(e, RenderDepth, 0);
    zox_setv(e, MaxRenderDepth, 0);
    zox_setv(e, NodeDepth, 0);
    zox_setv(e, BlockScale, vox_model_scale);
    // allocations
    zox_setv(e, VoxelNode, 0);
    zox_setv(e, VoxelNodeLock, 0);
    zox_setv(e, ColorRGBs, 0);
    return e;
}
