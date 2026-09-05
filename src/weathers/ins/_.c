entity spawn_skybox(
    ecs* world,
    entity camera,
    entity shader)
{
    // TODO: Make dynamic size
#ifdef zox_xr
    float skybox_size = 80;
#else
    float skybox_size = 666; // 1024;    // camera_far_distance * 0.95 roughly
#endif
    zox_instance(prefab_skybox);
    zox_name("skybox");
    zox_set_parent(world, e, camera);
    zox_setv(e, Scale1, skybox_size);
    zox_setv(e, LocalScale1, skybox_size);
    if (shader) {
        zox_add(e, MeshBasic3);
        // spawn_gpu_mesh(world, e);
        zox_set(e, ShaderLink, { shader });
        guint2 shader_value = zox_getv(shader, ShaderGPULink);
        guint gpu_material = spawn_gpu_material_program(shader_value);
        if (gpu_material) {
            zox_setv(e, MaterialGPULink, gpu_material);
            MaterialBasic3D attributes = create_MaterialBasic3D(gpu_material);
            zox_set_ptr(e, MaterialBasic3D, attributes);
            set_skybox_material_color(
                gpu_material,
                menu_sky_color,
                menu_sky_bottom_color);
        }
    }
    // skybox = e;
    return e;
}
