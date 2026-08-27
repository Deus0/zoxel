entity spawn_prefab_element(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("element");
    zox_add(e, Element);
    zox_add(e, Mesh);
    zox_add(e, TexturedMesh2);  // render system tag
    // Properties
    zox_setv(e, TransformMatrix, float4x4_identity);
    zox_setv(e, RenderDisabled, 0);
    zox_setv(e, Brightness, 1);
    zox_setv(e, Alpha, 1);
    // Texture
    zox_add(e, Texture);
    zox_setv(e, TextureData, 0);
    zox_setv(e, TextureSize, int2_zero);
    zox_setv(e, TextureGPULink, 0);
    // Mesh
    zox_add(e, Initialize);
    zox_setv(e, MeshAlignment, 0);
    zox_add(e, MeshIndicies);
    zox_add(e, MeshVertices2D);
    zox_add(e, MeshUVs);
    zox_setv(e, MeshGPULink, 0);
    zox_setv(e, UvsGPULink, 0);
    prefab_set_mesh_indicies(world, e, square_indicies, 6);
    prefab_set_mesh_uvs_float2(world, e, square_uvs, 4);
    prefab_set_mesh2D_vertices(world, e, square_vertices, 4);
    // prefab_set_mesh2D_vertices(world, e, NULL, 0);
    return e;
}
