entity spawn_prefab_static_chunk(ecs *world) {
    zox_prefab();
    zox_prefab_name("static_chunk");
    // Transforms
    zox_setv(e, TransformMatrix, float4x4_identity);
#ifdef zox_non_fragment_parent
    // zox_add(e, EcsParent);
#endif
    // Rendering
    zox_setv(e, Brightness, 1);
    zox_setv(e, MeshIndicies, 0);
    zox_setv(e, MeshVertices, 0);
    // Chunks
    zox_add(e, Chunk);
    zox_add(e, Chunk3);
    zox_setv(e, ChunkPosition, int3_zero);
    zox_setv(e, ChunkSize, int3_zero);
    zox_setv(e, ChunkNeighbors, { 0 });
    zox_setv(e, RenderDistance, 0);
    zox_setv(e, RenderDepth, 0);
    zox_setv(e, NodeDepth, 0);
    zox_setv(e, VoxelNode, 0);
    zox_setv(e, SidesOctree, 0);
    zox_setv(e, BlockScale, default_vox_scale);
    return e;
}
