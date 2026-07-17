// For chunks with children meshes
entity spawn_prefab_chunk_invisible(ecs *world) {
    zox_prefab();
    zox_prefab_name("chunk_invisible");
    zox_add_tag(e, Chunk);
    zox_add_tag(e, Chunk3);
    zox_prefab_set(e, InitializeEntity, { zox_dirty_trigger }); // load fast!
    zox_prefab_set(e, ChunkPosition, { int3_zero });
    zox_prefab_set(e, ChunkSize, { int3_zero });
    zox_prefab_set(e, BlockScale, { default_vox_scale });
    zox_prefab_set(e, ChunkNeighbors, { { 0 } } );
    // Voxel Data
    zox_prefab_set(e, NodeDepth, { 0 });
    zox_prefab_set(e, VoxelNode, { 0 });
    zox_prefab_set(e, VoxelNodeDirty, { zox_dirty_none });
    zox_prefab_set(e, BuildChunkSides, { 0 });
    // Render Data
    zox_prefab_set(e, SidesOctree, { 0 });
    zox_prefab_set(e, SidesOctreeDirty, { zox_dirty_none });
    zox_prefab_set(e, RenderDisabled, { 0 });   // used for frustum culling and general
    zox_prefab_set(e, RenderDistance, { render_depth_uninitialized });
    zox_prefab_set(e, RenderDistanceDirty, { zox_dirty_none });
    zox_prefab_set(e, RenderDepth, { render_depth_uninitialized });
    zox_prefab_set(e, RenderDepthDirty, { zox_dirty_none });
    // Transforms
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    // zox_prefab_set(e, BuildChunkMesh, { zox_dirty_none });
    return e;
}
