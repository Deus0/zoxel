entity spawn_prefab_element3(ecs *world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_prefab_name("element3");
    zox_add(e, Element3);
    zox_prefab_set(e, Initialize, { 1 });
    // Add Mesh
    zox_add(e, SingleMaterial);
    zox_add(e, TexturedMesh3);
    zox_prefab_set(e, MeshAlignment,{ zox_alignment_centre });
    zox_prefab_add(e, MeshIndicies);
    zox_prefab_add(e, MeshVertices);
    zox_prefab_add(e, MeshUVs);
    zox_prefab_add(e, MeshColorRGBs);
    prefab_set_mesh_indicies(world, e, square_indicies, 6);
    prefab_set_mesh_uvs_float2(world, e, square_uvs, 4);
    prefab_set_mesh_colors_rgb(world, e, (color_rgb) { 255, 255, 255 }, 4);
    // prefab_set_mesh3D_vertices(world, e, square_vertices, 4, (float2) { 0.04f, 0.04f });
    zox_prefab_set(e, TextureGPULink, { 0 });
    zox_prefab_set(e, MeshGPULink, { 0 });
    zox_prefab_set(e, UvsGPULink, { 0 });
    zox_prefab_set(e, ColorsGPULink, { 0 });
    zox_prefab_set(e, MeshDirty, { mesh_state_upload });
    zox_prefab_set(e, MeshRenderCount, { 6 });
    // Properties
    zox_prefab_set(e, Brightness, { 1 });
    zox_prefab_set(e, Alpha, { 1 });
    zox_prefab_set(e, RenderDisabled, { 0 });
    // Add Texture
    zox_add(e, Texture);
    zox_prefab_set(e, TextureData, { 0 });
    zox_prefab_set(e, TextureSize, { 0 });
    zox_prefab_set(e, TextureDirty, { 0 });
    return e;
}
