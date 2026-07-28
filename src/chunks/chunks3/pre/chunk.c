entity spawn_prefab_chunk(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("chunk");
    zox_prefab_set(e, Initialize, { 1 }); // load fast!
    zox_prefab_set(e, BuildChunkSides, { 0 });
    zox_prefab_set(e, BlockScale, { default_vox_scale });
    // Transforms
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    // Rendering
    zox_prefab_set(e, Brightness, { 1.0f });
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_prefab_set(e, MeshDirty, { 0 });
    zox_prefab_set(e, MeshIndicies, { 0 });
    zox_prefab_set(e, MeshRenderCount, { 0 });
    zox_prefab_set(e, MeshVertices, { 0 });
    zox_prefab_set(e, MeshGPULink, { { 0, 0 } });
    return e;
}

// A simple colored vox at position
entity spawn_chunk3(ecs *world, entity prefab, float3 position, float scale, byte depth, float block_scale, int3 size) {
    zox_instance(prefab);
    zox_name("chunk3")
    zox_set(e, Position3D, { position });
    // zox_set(e, Scale1, { scale });
    zox_set(e, BlockScale, { block_scale });
    zox_set(e, ChunkSize, { size });
    zox_set(e, NodeDepth, { depth });
    zox_set(e, RenderDepth, { depth });
    zox_add_tag(e, NoiseChunk);
    return e;
}
