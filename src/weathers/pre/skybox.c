entity spawn_prefab_skybox(ecs *world) {
    zox_prefab();
    zox_prefab_name("skybox");
    zox_add_tag(e, Skybox);
    zox_prefab_set(e, Initialize, { 1 });
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, Scale1, { 1 });
    zox_prefab_set(e, LocalScale1, { 1 });
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    zox_prefab_add(e, MeshIndicies);
    zox_prefab_add(e, MeshVertices);
    zox_prefab_set(e, MeshGPULink, { { 0, 0 } });
    zox_prefab_set(e, ShaderLink, { 0 });
    zox_prefab_set(e, MaterialGPULink, { 0 });
    zox_prefab_set(e, ShaderLink, { 0 });
    zox_prefab_set(e, MeshDirty, { mesh_state_upload });
    zox_prefab_set(e, Color, { color_white });
    zox_prefab_set(e, ColorRGB, { color_rgb_white });
    zox_prefab_add(e, SecondaryColorRGB);
    zox_prefab_set(e, Brightness, { 1 });
    zox_prefab_set(e, Alpha, { 1 });
    prefab_set_mesh_indicies(world, e, cube_indicies_inverted, cube_indicies_length);
    prefab_set_mesh_vertices_float(world, e, cube_vertices_inverted, cube_vertices_f_length);
    return e;
}
