entity spawn_prefab_element(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("element");
    zox_add_tag(e, Element);
    zox_add_tag(e, TexturedMesh2);  // render system tag
    // Properties
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_prefab_set(e, Brightness, { 1 });
    zox_prefab_set(e, Alpha, { 1 });
    // Mesh
    zox_prefab_set(e, Initialize, { 1 });
    zox_prefab_set(e, MeshDirty, { 0 });
    zox_prefab_set(e, MeshAlignment, { 0 });
    zox_prefab_add(e, MeshIndicies);
    zox_prefab_add(e, MeshVertices2D);
    zox_prefab_add(e, MeshUVs);
    zox_prefab_set(e, MeshGPULink, { 0 });
    zox_prefab_set(e, UvsGPULink, { 0 });
    prefab_set_mesh_indicies(world, e, square_indicies, 6);
    prefab_set_mesh_uvs_float2(world, e, square_uvs, 4);
    prefab_set_mesh2D_vertices(world, e, square_vertices, 4);
    // prefab_set_mesh2D_vertices(world, e, NULL, 0);
    // Texture
    zox_add_tag(e, Texture);
    zox_prefab_set(e, TextureData, { 0 });
    zox_prefab_set(e, TextureDirty, { 0 });
    zox_prefab_set(e, TextureSize, { int2_zero });
    zox_prefab_set(e, TextureGPULink, { 0 });
    return e;
}
