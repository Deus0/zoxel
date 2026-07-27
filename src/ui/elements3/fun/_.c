#include "can_render.c"
#include "popup.c"
// TODO: Spawn a Layout Canvas
// TODO: Spawn a button on it
// TODO: Raycast button by transforming ray that hits quad into the canvas

void add_ui_components_world(ecs *world, entity e, float2 mesh_scale) {
    zox_prefab_set(e, MeshDirty, { 0 });
    zox_prefab_set(e, Brightness, { 1 });
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_add_tag(e, Texture);
    zox_prefab_set(e, TextureData, { 0 });
    zox_prefab_set(e, TextureSize, { int2_zero });
    zox_prefab_set(e, TextureDirty, { 0 });
    // Generation of Frame Texture
    zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, GenerateTexture, { zox_generate_texture_run });
    zox_prefab_set(e, TextureGPULink, { 0 });
    zox_prefab_set(e, MeshGPULink, { 0 });
    zox_prefab_set(e, UvsGPULink, { 0 });
    zox_prefab_set(e, ColorsGPULink, { 0 });
    zox_prefab_add(e, MeshIndicies);
    zox_prefab_add(e, MeshVertices);
    zox_prefab_add(e, MeshUVs);
    zox_prefab_add(e, MeshColorRGBs);
    prefab_set_mesh_indicies(world, e, square_indicies, 6);
    prefab_set_mesh3D_vertices(world, e, square_vertices, 4, mesh_scale);
    prefab_set_mesh_uvs_float2(world, e, square_uvs, 4);
    prefab_set_mesh_colors_rgb(world, e, (color_rgb) { 255, 255, 255 }, 4);
    zox_prefab_set(e, MeshRenderCount, { 6 });
    zox_prefab_set(e, Initialize, { 1 });
    // zox_prefab_set(e, MeshDirty, { 1 });
    // NOTE: Needed for Uploading Mesh Textured
    zox_prefab_set(e, MeshRenderCount, { 0 });
    zox_prefab_set(e, MeshDirty, { 1 });
}
