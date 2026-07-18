void add_chunk_mesh_textured(ecs* world, entity e) {
    zox_prefab_set(e, MeshUVs, { 0 });
    zox_prefab_set(e, MeshColorRGBs, { 0 });
    zox_prefab_set(e, UvsGPULink, { 0 });
    zox_prefab_set(e, TextureGPULink, { 0 });
    zox_prefab_set(e, ColorsGPULink, { 0 });
    zox_prefab_set(e, MeshColorsGenerate, { 0 });
    zox_prefab_set(e, MeshColorsDirty, { 0 });
    zox_prefab_set(e, MeshReady, { 0 });
    zox_prefab_set(e, TexturedMeshDirty, { 0 });
}

entity spawn_prefab_chunk_mesh_textured(ecs* world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_prefab_set(e, MeshUVs, { 0 });
    zox_prefab_set(e, MeshColorRGBs, { 0 });
    zox_prefab_set(e, UvsGPULink, { 0 });
    zox_prefab_set(e, TextureGPULink, { 0 });
    zox_prefab_set(e, ColorsGPULink, { 0 });
    zox_prefab_set(e, MeshColorsGenerate, { 0 });
    zox_prefab_set(e, MeshColorsDirty, { 0 });
    zox_prefab_set(e, TexturedMeshDirty, { 0 });
    return e;
}
