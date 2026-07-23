entity spawn_prefab_chunk_mesh_textured(ecs* world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    // Render Data
    zox_prefab_set(e, MeshUVs, { 0 });
    // GPU Links
    zox_prefab_set(e, UvsGPULink, { 0 });
    // Events
    zox_prefab_set(e, MeshColorsGenerate, { 0 });
    zox_prefab_set(e, TexturedMeshDirty, { 0 });
    zox_prefab_set(e, MeshColorsDirty, { 0 });
    return e;
}
