entity spawn_prefab_cube(ecs *world) {
    zox_prefab();
    // zox_prefab_name("cube");
    zox_add_tag(e, Cube);
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, Scale1, { 0.125f });
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    zox_prefab_set(e, Color, { color_black });
    zox_prefab_set(e, Brightness, { 1 });
    zox_prefab_set(e, Alpha, { 1 });
    zox_prefab_set(e, RenderDisabled, { 0 });
    // Mesh
    zox_prefab_set(e, InitializeMesh, { 1 });
    zox_prefab_set(e, MeshDirty, { mesh_state_trigger });
    zox_prefab_set(e, MeshIndicies, { 0 });
    zox_prefab_set(e, MeshVertices, { 0 });
    zox_prefab_set(e, MeshGPULink, { { 0, 0 } });
    prefab_set_mesh_indicies(world, e, cube2_indicies, cube2_indicies_length);
    prefab_set_mesh_vertices_float3(world, e, cube2_vertices, cube2_vertices_length);
    // Material
    zox_prefab_set(e, ShaderLink, { 0 });
    zox_prefab_set(e, MaterialGPULink, { 0 });
    // zox_prefab_set(e, MeshRenderCount, { 0 });
    return e;
}
