entity spawn_prefab_cube_textured(
    ecs *world,
    entity prefab)
{
    int2 texture_size = int2_zero;
    zox_clone(prefab);
    zox_remove(e, MeshBasic3);
    // Properties
    zox_setv(e, RenderDisabled, 0);
    zox_setv(e, Brightness, 1);
    zox_setv(e, MaterialTextured3D, 0);
    // Texture
    zox_add(e, Texture);
    zox_setv(e, TextureData, 0);
    zox_setv(e, TextureSize, texture_size);
    zox_setv(e, TextureGPULink, 0);
    // Colors
    zox_add(e, MeshColorRGBs);
    zox_setv(e, ColorsGPULink, 0);
    prefab_set_mesh_colors_rgb(
        world,
        e,
        (color_rgb) { 255, 255, 255 },
        cube2_vertices_length);
    // UVs
    zox_add(e, MeshUVs);
    prefab_set_mesh_uvs_float2(
        world,
        e,
        cube2_uvs,
        cube2_uvs_length);
    zox_setv(e, UvsGPULink, 0);
    // NOTE: Needed for Uploading Mesh Textured
    zox_setv(e, MeshRenderCount, 0);
    zox_add(e, MeshDirty);
    return e;
}
