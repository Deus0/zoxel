entity spawn_prefab_chunk_mesh_textured(ecs* world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_prefab_set(e, MeshReady, { 0 });
    zox_prefab_set(e, RenderDisabled, { 0 }); // 1 });
    zox_prefab_set(e, MeshUVs, { 0 });
    zox_prefab_set(e, MeshColorRGBs, { 0 });
    zox_prefab_set(e, MeshColorsGenerate, { zox_dirty_none });
    zox_prefab_set(e, MeshColorsDirty, { zox_dirty_none });
    add_gpu_uvs(world, e);
    add_gpu_texture(world, e);
    add_gpu_colors(world, e);
    return e;
}
