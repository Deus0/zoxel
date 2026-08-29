// TODO: Use chunk bbasse... wth lol
entity spawn_prefab_vox(ecs *world, byte depth) {
    zox_prefab();
    zox_prefab_name("vox");
    zox_add(e, Vox);
    zox_add(e, Chunk);
    zox_add(e, Chunk3);
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
    // Transforms
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, Scale1, { 1 });
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    // Rendering
    zox_prefab_set(e, Brightness, { 1.0f });
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_add(e, Mesh);
    zox_prefab_set(e, MeshIndicies, { 0 });
    zox_prefab_set(e, MeshRenderCount, { 0 });
    zox_prefab_set(e, MeshVertices, { 0 });
    zox_prefab_set(e, MeshGPULink, { { 0, 0 } });
    // zox_prefab_set(e, BuildMesh, { 0 });
    // vox
    zox_set(e, BlockScale, { 0 }); // vox_model_scale });
    zox_set(e, NodeDepth, { 0 }); // depth
    zox_add(e, ColorChunk);
    zox_prefab_set(e, ColorRGBs, { 0 });
    zox_prefab_set(e, MeshColorRGBs, { 0 });
    zox_prefab_set(e, ColorsGPULink, { 0 });
    // Events
    zox_add(e, PreInitialize);
    // zox_setv(e, MeshDirty, 0);
    return e;
}

entity spawn_prefab_static_vox(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("vox");
    // vox
    zox_setv(e, BlockScale, 0); // vox_model_scale });
    zox_setv(e, NodeDepth, 0); // depth
    zox_add(e, ColorChunk);
    zox_setv(e, ColorRGBs, 0);
    zox_setv(e, MeshColorRGBs, 0);
    zox_setv(e, ColorsGPULink, 0);
    return e;
}
