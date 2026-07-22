// TODO: Just use base mesh instead
entity spawn_prefab_chunk_mesh(ecs *world) {
    zox_prefab();
    zox_prefab_name("chunk_mesh");
    zox_add_tag(e, ChunkMesh);
    zox_prefab_set(e, Active, { 1 });
    if (!zox_tst_remove_deactivates) {
        zox_prefab_set(e, DeactivateDelay, { 0 });
    }
    zox_prefab_set(e, BuildChunkMesh, { zox_build_chunk_mesh_run });
    // Transforms
    zox_add_tag(e, DisableTransform);
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    // Render Data
    zox_prefab_set(e, RenderDisabled, { 1 });
    zox_prefab_set(e, RenderDepth, { 0 });  // used to tell which depth the mesh is at
    // Nesh Data
    zox_prefab_set(e, MeshIndicies, { 0 });
    zox_prefab_set(e, MeshVertices, { 0 });
    zox_prefab_set(e, MeshGPULink, { { 0, 0 } });
    zox_prefab_set(e, MeshRenderCount, { 0 });
    // zox_prefab_set(e, MeshDirty, { 0 });
    return e;
}
