entity spawn_prefab_vox(ecs *world, byte depth) {
    zox_prefab();
    zox_prefab_name("vox");
    zox_add_tag(e, Vox);
    zox_add_tag(e, Chunk);
    zox_add_tag(e, Chunk3);
    zox_prefab_set(e, ChunkPosition, { int3_zero });
    zox_prefab_set(e, ChunkSize, { int3_zero });
    zox_prefab_set(e, ChunkNeighbors, { { 0 } } );
    // render_depth_uninitialized
    zox_prefab_set(e, RenderDistance, { 0 });
    zox_prefab_set(e, RenderDepth, { 0 });
    zox_prefab_set(e, BlockScale, { default_vox_scale });
    // allocations
    zox_prefab_set(e, NodeDepth, { 0 });
    zox_prefab_set(e, VoxelNode, { 0 });
    zox_prefab_set(e, SidesOctree, { 0 });
    // Dirty State
    zox_prefab_set(e, VoxelNodeDirty, { 0 });
    zox_prefab_set(e, RenderDistanceDirty, { 0 });
    zox_prefab_set(e, RenderDepthDirty, { 0 });
    // Transforms
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, Scale1, { 1 });
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    // Rendering
    zox_prefab_set(e, Brightness, { 1.0f });
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_prefab_set(e, MeshIndicies, { 0 });
    zox_prefab_set(e, MeshRenderCount, { 0 });
    zox_prefab_set(e, MeshVertices, { 0 });
    zox_prefab_set(e, MeshGPULink, { { 0, 0 } });
    zox_prefab_set(e, Initialize, { 1 });
    zox_prefab_set(e, MeshDirty, { 0 });
    zox_prefab_set(e, BuildChunkSides, { 0 });
    zox_prefab_set(e, BuildChunkMesh, { 0 });
    // vox
    zox_set(e, BlockScale, { 0 }); // vox_model_scale });
    zox_set(e, NodeDepth, { 0 }); // depth
    zox_add_tag(e, ColorChunk);
    zox_prefab_set(e, ColorRGBs, { 0 });
    zox_prefab_set(e, MeshColorRGBs, { 0 });
    zox_prefab_set(e, ColorsGPULink, { 0 });
    return e;
}

entity spawn_prefab_static_vox(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("vox");
    // vox
    zox_set(e, BlockScale, { 0 }); // vox_model_scale });
    zox_set(e, NodeDepth, { 0 }); // depth
    zox_add_tag(e, ColorChunk);
    zox_prefab_set(e, ColorRGBs, { 0 });
    zox_prefab_set(e, MeshColorRGBs, { 0 });
    zox_prefab_set(e, ColorsGPULink, { 0 });
    return e;
}
