// TODO: Just use base mesh instead
entity spawn_prefab_chunk_mesh(ecs* world) {
    zox_prefab();
    zox_prefab_name("chunk_mesh");
    zox_add(e, Mesh);
    zox_add(e, ChunkMesh);
    // Data
    zox_setv(e, TransformMatrix, float4x4_identity);
    zox_setv(e, RenderDepth, 0);
    zox_setv(e, RenderDisabled, 0)
    // Mesh Data
    zox_add(e, MeshIndicies);
    zox_add(e, MeshVertices);
    zox_add(e, MeshColorRGBs);
    // Gpu Links
    zox_setv(e, MeshGPULink, 0);
    zox_setv(e, ColorsGPULink, 0);
    zox_setv(e, MeshRenderCount, 0);
    // Starting Events
    zox_setv(e, Initialize, 1);
    zox_setv(e, BuildMesh, zox_build_chunk_mesh_run);
    return e;
}
