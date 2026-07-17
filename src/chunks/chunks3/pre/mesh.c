// TODO: Just use base mesh instead
entity spawn_prefab_chunk_mesh(ecs *world) {
    zox_prefab();
    zox_prefab_name("chunk_invisible");
    zox_add_tag(e, ChunkMesh);
    zox_prefab_set(e, InitializeEntity, { zox_dirty_trigger }); // load fast!
    zox_prefab_set(e, Busy, { 0 }); // load fast!
    // Transforms
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    // zox_prefab_set(e, LocalPosition3D, { float3_zero });
    // Render Data
    zox_prefab_set(e, RenderDepth, { 0 });  // used to tell which depth the mesh is at
    zox_prefab_set(e, BuildChunkMesh, { zox_dirty_trigger });
    // Nesh Data
    zox_prefab_set(e, MeshDirty, { 0 });
    zox_prefab_set(e, MeshIndicies, { 0 });
    zox_prefab_set(e, MesnRenderCount, { 0 });
    zox_prefab_set(e, MeshVertices, { 0 });
    zox_prefab_set(e, MeshGPULink, { { 0, 0 } });
    return e;
}
