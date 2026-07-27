entity spawn_prefab_static_chunk(ecs *world) {
    zox_prefab();
    zox_prefab_name("static_chunk");
    zox_add_tag(e, Chunk);
    zox_add_tag(e, Chunk3);
    // zox_prefab_set(e, Initialize, { zox_dirty_trigger });
    zox_prefab_set(e, ChunkPosition, { int3_zero });
    zox_prefab_set(e, ChunkSize, { int3_zero });
    zox_prefab_set(e, ChunkNeighbors, { { 0 } } );
    zox_prefab_set(e, RenderDistance, { 0 });
    zox_prefab_set(e, RenderDepth, { 0 });
    zox_prefab_set(e, NodeDepth, { 0 });
    zox_prefab_set(e, VoxelNode, { 0 });
    zox_prefab_set(e, SidesOctree, { 0 });
    zox_prefab_set(e, VoxelNodeDirty, { 0 });
    zox_prefab_set(e, RenderDistanceDirty, { 0 });
    zox_prefab_set(e, RenderDepthDirty, { 0 });
    zox_prefab_set(e, BuildChunkMesh, { 0 });
    zox_prefab_set(e, BuildChunkSides, { 0 });
    zox_prefab_set(e, BlockScale, { default_vox_scale });
    // Rendering
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    zox_prefab_set(e, Brightness, { 1.0f });
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_prefab_set(e, MeshRenderCount, { 0 });
    zox_prefab_set(e, MeshDirty, { 0 });
    zox_prefab_set(e, MeshIndicies, { 0 });
    zox_prefab_set(e, MeshVertices, { 0 });
    zox_prefab_set(e, MeshGPULink, { { 0, 0 } });
    return e;
}
