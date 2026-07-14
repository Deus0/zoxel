entity spawn_prefab_cube_textured(ecs *world, entity prefab) {
    int2 texture_size = int2_zero; // { 16, 16 };
    // zox_prefab_child(prefab)
    zox_clone(prefab);
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_prefab_set(e, Brightness, { 1 });
    zox_prefab_set(e, Color, { color_red });
    zox_add_tag(e, Texture);
    zox_prefab_set(e, TextureData, { 0, NULL });
    zox_prefab_set(e, TextureSize, { texture_size });
    zox_prefab_set(e, TextureDirty, { 0 });
    zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, GenerateTexture, { zox_generate_texture_run });
    add_noise_texture(world, e);
    zox_remove_tag(e, MeshBasic3D);
    zox_add_tag(e, TexturedMesh3D);
    zox_prefab_set(e, MaterialTextured3D, { 0 });
    zox_prefab_set(e, UvsGPULink, { 0 });
    zox_prefab_set(e, MeshGPULink, { 0 });
    zox_prefab_set(e, MaterialGPULink, { 0 });
    zox_prefab_set(e, ColorsGPULink, { 0 });
    zox_prefab_add(e, MeshColorRGBs);
    zox_prefab_set(e, TextureGPULink, { 0 });
    prefab_set_mesh_indicies(world, e, cube2_indicies, cube2_indicies_length);
    prefab_set_mesh_vertices_float3(world, e, cube2_vertices, cube2_vertices_length);
    prefab_set_mesh_colors_rgb(world, e, (color_rgb) { 255, 255, 255 }, cube2_vertices_length);
    add_gpu_texture(world, e);
    prefab_set_mesh_uvs_float2(world, e, cube2_uvs, cube2_uvs_length);
    return e;
}
