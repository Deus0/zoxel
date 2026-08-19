// For chunks with children meshes
entity spawn_prefab_chunk_invisible(ecs *world) {
    zox_prefab();
    zox_prefab_name("chunk_invisible");
    zox_add(e, Chunk);
    zox_add(e, Chunk3);
    zox_prefab_set(e, ChunkPosition, { int3_zero });
    zox_prefab_set(e, ChunkSize, { int3_zero });
    zox_prefab_set(e, BlockScale, { default_vox_scale });
    zox_prefab_set(e, ChunkNeighbors, { { 0 } } );
    zox_prefab_set(e, NodeDepth, { 0 });
    zox_prefab_set(e, VoxelNode, { 0 });
    zox_prefab_set(e, SidesOctree, { 0 });
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    zox_prefab_set(e, RenderDistance, { 0 });
    zox_prefab_set(e, RenderDepth, { 0 });
    zox_prefab_set(e, RenderDisabled, { 0 });
    // Events
    zox_prefab_set(e, VoxelNodeDirty, { 0 });
    // zox_prefab_set(e, BuildChunkSides, { 0 });
    return e;
}
