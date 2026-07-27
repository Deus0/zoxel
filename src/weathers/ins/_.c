

entity spawn_skybox(ecs *world, entity camera, entity shader) {
    // TODO: Make dynamic size
    float skybox_size = 766; // 1024;    // camera_far_distance * 0.95 roughly
    zox_instance(prefab_skybox);
    zox_name("skybox");
    zox_set(e, Scale1, { skybox_size });
    zox_set(e, LocalScale1, { skybox_size });
    zox_set_parent(world, e, camera);
    zox_setv(e, Initialize, 1);
    if (shader) {
        zox_add_tag(e, MeshBasic3D);
        // spawn_gpu_mesh(world, e);
        zox_set(e, ShaderLink, { shader });
        guint2 shader_skybox_value = get_shader_value(world, shader);
        guint material = spawn_gpu_material(world, e, shader_skybox_value);
        if (!material) {
            zox_log("[spawn_skybox] Failed");
            zox_delete(e);
            return 0;
        }
        MaterialBasic3D attributes = create_MaterialBasic3D(material);
        zox_set_ptr(e, MaterialBasic3D, attributes);
        set_skybox_material_color(material, menu_sky_color, menu_sky_bottom_color);
    }
    skybox = e;
    return e;
}

void spawn_weather(ecs *world, entity app) {
    entity main_camera = main_cameras[0];
    prefab_skybox = spawn_skybox(world, main_camera, shader_skybox);
    set_skybox_colors(world, menu_sky_color, menu_sky_bottom_color);
}
