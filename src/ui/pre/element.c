entity spawn_prefab_element(ecs *world, entity prefab) {
    byte alignment = zox_alignment_centre;
    zox_prefab_child(prefab);
    zox_prefab_name("element");
    zox_add_tag(e, Element);
    zox_add_tag(e, TexturedMesh2);
    // Properties
    // zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_prefab_set(e, Brightness, { 1 });
    zox_prefab_set(e, Alpha, { 1 });
    // Texture
    zox_add_tag(e, Texture);
    zox_prefab_set(e, TextureData, { 0 });
    zox_prefab_set(e, TextureDirty, { 0 });
    zox_prefab_set(e, TextureSize, { int2_zero });
    zox_prefab_set(e, TextureGPULink, { 0 });
    // Mesh
    zox_prefab_set(e, MeshAlignment, { alignment });
    zox_prefab_add(e, MeshIndicies);
    zox_prefab_add(e, MeshVertices2D);
    zox_prefab_add(e, MeshUVs);
    zox_prefab_set(e, MeshGPULink, { 0 });
    zox_prefab_set(e, UvsGPULink, { 0 });
    prefab_set_mesh_indicies(world, e, square_indicies, 6);
    prefab_set_mesh_uvs_float2(world, e, square_uvs, 4);
    // prefab_set_mesh2D_vertices(world, e, NULL, 0);
    prefab_set_mesh2D_vertices(world, e, square_vertices, 4);
    // zox_mut_begin(e, MeshVertices2D, verts);
    // set_mesh_vertices_scale2D(verts, get_aligned_mesh2D(alignment), 4, float2_single(0.05f));
    // Events
    zox_prefab_set(e, Initialize, { 1 });
    zox_prefab_set(e, MeshDirty, { 0 }); // mesh_state_upload | 0
#ifdef zox_disable_initialize_removal
    zox_add(e, Busy);
#endif
    return e;
}
