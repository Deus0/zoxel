// data only
entity spawn_prefab_chunk_base(ecs *world) {
    // TODO: Calculate this from max resolution!
    int3 size = int3_single(32);
    zox_prefab();
    zox_prefab_name("chunk_base");
    zox_add_tag(e, Chunk);
    zox_add_tag(e, Chunk3);
    zox_prefab_set(e, ChunkPosition, { int3_zero });
    zox_prefab_set(e, ChunkSize, { size });
    zox_prefab_set(e, ChunkNeighbors, { { 0 } } );
    // states
    zox_prefab_set(e, ChunkMeshDirty, { zox_dirty_none });
    // allocations
    zox_prefab_set(e, NodeDepth, { 0 });
    zox_prefab_set(e, VoxelNode, { 0 });
    zox_prefab_set(e, VoxelNodeDirty, { zox_dirty_none });
    zox_prefab_set(e, SidesOctree, { 0 });
    zox_prefab_set(e, SidesOctreeDirty, { zox_dirty_none });
    // Render
    zox_prefab_set(e, RenderDistance, { render_depth_spawning });
    zox_prefab_set(e, RenderDistanceDirty, { zox_dirty_none });
    zox_prefab_set(e, RenderDepth, { render_depth_spawning });
    zox_prefab_set(e, RenderDepthDirty, { zox_dirty_none });
    return e;
}
