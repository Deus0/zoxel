// data only
entity spawn_prefab_chunk_base(ecs *world) {
    zox_prefab();
    zox_prefab_name("chunk");
    zox_add_tag(e, Chunk);
    zox_add_tag(e, Chunk3);
    zox_prefab_set(e, ChunkPosition, { int3_zero });
    zox_prefab_set(e, ChunkSize, { int3_zero });
    zox_prefab_set(e, ChunkNeighbors, { { 0 } } );
    // render_depth_uninitialized
    zox_prefab_set(e, RenderDistance, { 0 });
    zox_prefab_set(e, RenderDepth, { 0 });
    // allocations
    zox_prefab_set(e, NodeDepth, { 0 });
    zox_prefab_set(e, VoxelNode, { 0 });
    zox_prefab_set(e, SidesOctree, { 0 });
    // Dirty State
    zox_prefab_set(e, VoxelNodeDirty, { 0 });
    zox_prefab_set(e, SidesOctreeDirty, { 0 });
    zox_prefab_set(e, RenderDistanceDirty, { 0 });
    zox_prefab_set(e, RenderDepthDirty, { 0 });
    zox_prefab_set(e, BuildChunkMesh, { 0 });
    return e;
}
