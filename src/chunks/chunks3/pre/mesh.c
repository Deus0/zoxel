// TODO: Just use base mesh instead
entity spawn_prefab_chunk_mesh(ecs* world) {
    zox_prefab();
    zox_prefab_name("chunk_mesh");
    zox_add(e, ChunkMesh);
    // Data
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    zox_prefab_set(e, RenderDepth, { 0 });
    zox_prefab_set(e, RenderDisabled, { 0 })
    // Mesh Data
    zox_prefab_set(e, MeshIndicies, { 0 });
    zox_prefab_set(e, MeshVertices, { 0 });
    zox_prefab_set(e, MeshColorRGBs, { 0 });
    // Gpu Links
    zox_prefab_set(e, MeshGPULink, { { 0, 0 } });
    zox_prefab_set(e, ColorsGPULink, { 0 });
    zox_prefab_set(e, MeshRenderCount, { 0 });
    // Starting Events
    zox_prefab_set(e, Initialize, { 1 });
    zox_prefab_set(e, BuildMesh, { zox_build_chunk_mesh_run });
    return e;
}
