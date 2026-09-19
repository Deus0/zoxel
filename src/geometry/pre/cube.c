entity spawn_prefab_cube(ecs *world) {
    zox_prefab();
    zox_prefab_name("cube");
    zox_add(e, Cube);
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, float4_identity);
    zox_setv(e, Scale1, 0.125f);
    zox_setv(e, TransformMatrix, float4x4_identity);
    zox_setv(e, Color, color_black);
    zox_setv(e, Brightness, 1);
    zox_setv(e, Alpha, 1);
    zox_setv(e, RenderDisabled, 0);
    // Mesh
    zox_add(e, Mesh);
    zox_setv(e, MeshIndicies, 0);
    zox_setv(e, MeshVertices, 0);
    zox_set(e, MeshGPULink, { { 0, 0 } });
    prefab_set_mesh_indicies(
        world,
        e,
        cube2_indicies,
        cube2_indicies_length);
    prefab_set_mesh_vertices_float3(
        world,
        e,
        cube2_vertices,
        cube2_vertices_length);
    // Material
    zox_setv(e, MaterialGPULink, 0);
    zox_add(e, PreInitialize);
    zox_add(e, MeshDirty);
    return e;
}
