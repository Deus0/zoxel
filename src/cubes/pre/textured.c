entity spawn_prefab_cube_textured(ecs *world, entity prefab) {
    int2 texture_size = int2_zero; // { 16, 16 };
    // zox_prefab_child(prefab);
    zox_clone(prefab);
    // Properties
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_prefab_set(e, Brightness, { 1 });
    // zox_prefab_set(e, Color, { color_red });
    // zox_prefab_set(e, TextureDirty, { 0 });
    // zox_prefab_set(e, Seed, { 666 });
    // zox_prefab_set(e, GenerateTexture, { zox_generate_texture_run });
    // add_noise_texture(world, e);
    zox_remove(e, MeshBasic3D);
    zox_prefab_set(e, MaterialTextured3D, { 0 });
    // Texture
    zox_add(e, Texture);
    zox_prefab_set(e, TextureData, { 0 });
    zox_prefab_set(e, TextureSize, { texture_size });
    zox_prefab_set(e, TextureGPULink, { 0 });
    // Colors
    zox_prefab_add(e, MeshColorRGBs);
    zox_prefab_set(e, ColorsGPULink, { 0 });
    prefab_set_mesh_colors_rgb(world, e, (color_rgb) { 255, 255, 255 }, cube2_vertices_length);
    // UVs
    zox_prefab_add(e, MeshUVs);
    prefab_set_mesh_uvs_float2(world, e, cube2_uvs, cube2_uvs_length);
    zox_prefab_set(e, UvsGPULink, { 0 });
    // NOTE: Needed for Uploading Mesh Textured
    zox_set(e, MeshRenderCount, { 0 });
    zox_set(e, MeshDirty, { mesh_state_upload });
    return e;
}
