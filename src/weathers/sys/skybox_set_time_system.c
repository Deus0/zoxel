// Sets the skybox's time (its clouds drift) and its camera's rotation: the skybox is a child of the camera and turns
//  with it, the shader turns its directions back into the world (the clouds and gradient stay put as the view turns)
void skybox_set_time_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin()
    zox_sys_in(MaterialGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MaterialGPULink, materialGPULink)
        const uint material = materialGPULink->value;
        if (!material) {
            continue;
        }
        const entity camera = zox_get_parent(world, e);
        const float4 rotation = zox_valid(camera) && zox_has(camera, Rotation3D) ?
            zox_getv(camera, Rotation3D) : quaternion_identity;
        zox_gpu_material(material);
        zox_gpu_float(zox_gpu_get_material_property(material, "time"), (float) zox_current_time);
        zox_gpu_float4(zox_gpu_get_material_property(material, "camera_rotation"), rotation);
        zox_gpu_material(0);
    }
    zox_sys_on_end();
} zoxd_system(skybox_set_time_system);
