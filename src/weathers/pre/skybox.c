entity spawn_prefab_skybox(ecs *world) {
    zox_prefab();
    zox_prefab_name("skybox");
    zox_add_tag(e, Skybox);
    // Transform
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, Scale1D, { 1 });
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    // Render
    zox_prefab_set(e, Color, { color_white });
    zox_prefab_set(e, ColorRGB, { color_rgb_white });
    zox_prefab_add(e, SecondaryColorRGB);
    zox_prefab_set(e, Brightness, { 1 });
    zox_prefab_set(e, Alpha, { 1 });
    zox_prefab_set(e, MeshDirty, { mesh_state_trigger });
    {
        zox_prefab_add(e, MeshIndicies);
        zox_prefab_add(e, MeshVertices);
        add_gpu_mesh(world, e);
        zox_prefab_set(e, ShaderLink, { 0 });
        add_gpu_material(world, e);
        prefab_set_mesh_indicies(world, e, cube_indicies_inverted, cube_indicies_length);
        prefab_set_mesh_vertices_float(world, e, cube_vertices_inverted, cube_vertices_f_length);
    }
    return e;
}

void set_skybox_colors(ecs *world, color_rgb top_color, color_rgb bottom_color) {
    if (!skybox) {
        return;
    }
    if (override_sky) {
        top_color = override_sky_fill;
        bottom_color = override_sky_fill;
    }
    zox_set(skybox, ColorRGB, { top_color });
    zox_set(skybox, SecondaryColorRGB, { bottom_color });
    zox_geter_value(skybox, MaterialGPULink, uint, material);
    set_skybox_material_color(material, top_color, bottom_color);
}

entity spawn_skybox(ecs *world, entity camera, entity shader) {
    // TODO: Make dynamic size
    float skybox_size = 766; // 1024;    // camera_far_distance * 0.95 roughly
    zox_instance(prefab_skybox);
    zox_name("skybox");
    zox_set(e, Scale1D, { skybox_size });
    zox_set_parent(world, e, camera);
    // zox_set(e, ShadowLink, { camera });
    if (shader) {
        zox_add_tag(e, MeshBasic3D);
        spawn_gpu_mesh(world, e);
        zox_set(e, ShaderLink, { shader });
        guint2 shader_skybox_value = get_shader_value(world, shader);
        guint material = spawn_gpu_material(world, e, shader_skybox_value);
        if (!material) {
            zox_log("[spawn_skybox] Failed");
            zox_delete(e);
            return 0;
        }
        set_skybox_material_color(material, menu_sky_color, menu_sky_bottom_color);
    }
    skybox = e;
    return e;
}
