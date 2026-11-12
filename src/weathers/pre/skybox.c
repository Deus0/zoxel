entity spawn_prefab_skybox(ecs *world) {
    zox_prefab();
    zox_prefab_name("skybox");
    zox_add(e, Skybox);
    zox_add(e, IgnoreParentRotation);
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, float4_identity);
    zox_setv(e, Scale1, 1);
    zox_setv(e, LocalPosition3D, float3_zero);
    zox_setv(e, LocalRotation3D, float4_identity);
    zox_setv(e, LocalScale1, 1);
    zox_setv(e, TransformMatrix, float4x4_identity);
    zox_add(e, Mesh);
    zox_add(e, MeshIndicies);
    zox_add(e, MeshVertices);
    zox_set(e, MeshGPULink, { { 0, 0 } });
    zox_setv(e, MaterialGPULink, 0);
    zox_setv(e, Color, color_white);
    zox_setv(e, ColorRGB, color_rgb_white);
    zox_add(e, SecondaryColorRGB);
    zox_setv(e, Brightness, 1);
    zox_setv(e, Alpha, 1);
    prefab_set_mesh_indicies(
        world,
        e,
        cube_indicies_inverted,
        cube_indicies_length);
    prefab_set_mesh_vertices_float(
        world,
        e,
        cube_vertices_inverted,
        cube_vertices_f_length);
    zox_add(e, MeshBasic3);
    zox_add(e, MaterialBasic3D);
    zox_add(e, PreInitialize);
    zox_add(e, MeshDirty);
    return e;
}
