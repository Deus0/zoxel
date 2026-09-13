// data only
entity spawn_prefab_chunk_base(ecs *world) {
    zox_prefab();
    zox_prefab_name("chunk");
    zox_add(e, Chunk);
    zox_add(e, Chunk3);
    zox_setv(e, NodeDepth, 0);
    zox_setv(e, ChunkPosition, int3_zero);
    zox_setv(e, ChunkSize, int3_zero);
    zox_setv(e, ChunkNeighbors, { 0 });
    zox_setv(e, RenderDistance, 0);
    zox_setv(e, RenderDepth, 0);
    // allocations
    zox_setv(e, VoxelNode, 0);
    zox_setv(e, VoxelNodeLock, 0);
    zox_setv(e, SidesOctree, 0);
#ifdef zox_non_fragment_parent
    zox_add(e, EcsParent);
#endif
    return e;
}
