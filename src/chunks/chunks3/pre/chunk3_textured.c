// For chunks with children meshes
entity spawn_prefab_chunk_invisible(ecs *world) {
    zox_prefab();
    zox_prefab_name("chunk_invisible");
    zox_add(e, Chunk);
    zox_add(e, Chunk3);
    zox_setv(e, ChunkPosition, int3_zero);
    zox_setv(e, ChunkSize, int3_zero);
    zox_setv(e, BlockScale, default_vox_scale);
    zox_setv(e, NodeDepth, 0);
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, float4_identity);
    zox_setv(e, TransformMatrix, float4x4_identity);
    zox_setv(e, RenderDistance, 0);
    zox_setv(e, RenderDepth, 0);
    zox_setv(e, RenderDisabled, 0);
    zox_setv(e, ChunkNeighbors, { 0 });
    zox_setv(e, VoxelNode, 0);
    zox_setv(e, VoxelNodeLock, 0);
    zox_setv(e, SidesOctree, 0);
    zox_setv(e, SidesOctreeLock, 0);
    return e;
}

entity spawn_prefab_chunk_textured(
    ecs* world,
    entity prefab)
{
    entity e = zox_prefab_from_parent(world, prefab);
    zox_set_unique_name(e, "chunk3_textured");
    zox_add(e, ChunkTextured);
    zox_add(e, ChunkMeshTimer);
    zox_setv(e, Bounds3D, float3_single(1));
    zox_setv(e, VoxelNodeQueue, 0);
    zox_setv(e, BlockDamageQueue, 0);
    // Events
    zox_add(e, FindNeighbors);
    zox_add(e, PreInitialize);
    return e;
}
