// data only
entity spawn_prefab_chunk_base(ecs *world) {
    zox_prefab();
    zox_prefab_name("chunk");
    zox_add_tag(e, Chunk);
    zox_add_tag(e, Chunk3);
    zox_prefab_set(e, ChunkPosition, { int3_zero });
    zox_prefab_set(e, ChunkSize, { int3_zero });
    zox_prefab_set(e, ChunkNeighbors, { { 0 } } );
    zox_prefab_set(e, RenderDistance, { render_depth_uninitialized });
    zox_prefab_set(e, RenderDepth, { render_depth_uninitialized });
    // allocations
    zox_prefab_set(e, NodeDepth, { 0 });
    zox_prefab_set(e, VoxelNode, { 0 });
    zox_prefab_set(e, SidesOctree, { 0 });
    // Dirty State
    zox_prefab_set(e, ChunkMeshDirty, { zox_dirty_none });
    zox_prefab_set(e, VoxelNodeDirty, { zox_dirty_none });
    zox_prefab_set(e, SidesOctreeDirty, { zox_dirty_none });
    zox_prefab_set(e, RenderDistanceDirty, { zox_dirty_none });
    zox_prefab_set(e, RenderDepthDirty, { zox_dirty_none });
    return e;
}
