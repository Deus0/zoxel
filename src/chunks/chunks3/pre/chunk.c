entity spawn_prefab_chunk(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("chunk");
    zox_setv(e, BlockScale, default_vox_scale );
    // Transforms
    zox_setv(e, Position3D, float3_zero );
    zox_setv(e, Rotation3D, float4_identity );
    zox_setv(e, TransformMatrix, float4x4_identity );
    // Rendering
    zox_setv(e, Brightness, 1);
    zox_setv(e, RenderDisabled, 0);
    zox_add(e, Mesh);
    zox_setv(e, MeshIndicies, 0);
    zox_setv(e, MeshRenderCount, 0);
    zox_setv(e, MeshVertices, 0);
    zox_setv(e, MeshGPULink, 0);
    // Events
    zox_add(e, PreInitialize);
    return e;
}

// A simple colored vox at position
entity spawn_chunk3(
    ecs *world,
    entity prefab,
    float3 position,
    float scale,
    byte depth,
    float block_scale,
    int3 size)
{
    zox_instance(prefab);
    zox_name("chunk3")
    zox_setv(e, Position3D, position);
    zox_setv(e, Scale1, scale);
    zox_setv(e, BlockScale, block_scale);
    zox_setv(e, ChunkSize, size);
    zox_setv(e, NodeDepth, depth);
    zox_setv(e, RenderDepth, depth);
    zox_add(e, NoiseChunk);
    return e;
}

entity spawn_prefab_chunk_textured(ecs* world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_set_unique_name(e, "chunk_textured");
    zox_add(e, ChunkTextured);
    zox_setv(e, Bounds3D, float3_single(1));
    zox_setv(e, VoxelNodeQueue, 0);
    zox_setv(e, BlockDamageQueue, 0);
    // Links
    zox_add(e, BlockManagerLink);
    zox_add(e, PreparingMesh);
    zox_add(e, ActiveMesh);
    zox_add(e, ChunkMeshTimer);
    // Events
    zox_add(e, PreInitialize);
    return e;
}
