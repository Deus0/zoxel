entity spawn_prefab_element3(ecs *world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_prefab_name("element3");
    zox_add_tag(e, Element3);
    zox_add_tag(e, SingleMaterial);
    zox_prefab_set(e, FillColor, {{ 0, 255, 0, 255 }});
    zox_prefab_set(e, LocalPosition3D, { float3_zero });
    zox_prefab_set(e, LocalRotation3D, { quaternion_identity });
    // Texture
    zox_prefab_set(e, Seed, { 666 });
    zox_add_tag(e, Texture);
    zox_prefab_set(e, TextureData, { 0 });
    zox_prefab_set(e, TextureSize, { int2_zero });
    zox_prefab_set(e, TextureDirty, { 0 });
    // Generation of Frame Texture
    // zox_prefab_set(e, MeshDirty, { 0 });
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_prefab_set(e, MeshRenderCount, { 6 });
    zox_prefab_set(e, Brightness, { 1 });
    zox_prefab_set(e, Alpha, { 1 });
    // Mesh Data
    zox_prefab_add(e, MeshIndicies);
    zox_prefab_add(e, MeshVertices);
    zox_prefab_add(e, MeshUVs);
    zox_prefab_add(e, MeshColorRGBs);
    prefab_set_mesh_indicies(world, e, square_indicies, 6);
    prefab_set_mesh3D_vertices(world, e, square_vertices, 4, (float2) { 0.04f, 0.04f });
    prefab_set_mesh_uvs_float2(world, e, square_uvs, 4);
    prefab_set_mesh_colors_rgb(world, e, (color_rgb) { 255, 255, 255 }, 4);
    // GPU Links
    zox_prefab_set(e, TextureGPULink, { 0 });
    zox_prefab_set(e, MeshGPULink, { 0 });
    zox_prefab_set(e, UvsGPULink, { 0 });
    zox_prefab_set(e, ColorsGPULink, { 0 });
    // Events
    zox_prefab_set(e, GenerateTexture, { zox_generate_texture_run });
    zox_prefab_set(e, MeshDirty, { 1 });
    zox_prefab_set(e, Initialize, { 1 });
    return e;
}
