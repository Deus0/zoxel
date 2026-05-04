// This renders Character2Ds and any other 2D assets
// This is called in Render3D due to transform needed
zox_sys2(RenderMaterial2DSystem) {

    if (!global_mesh2.x || !global_mesh2.y) {
        if (!global_mesh2.x) {
            zox_log_error("global_mesh2.x 0")
        }
        if (!global_mesh2.y) {
            zox_log_error("global_mesh2.y 0")
        }
        return;
    }

    float position_z = 0.1f;

    zox_gpu_enable_blend();
    // zox_gpu_disable_depth_test();
    // zox_gpu_disable_culling();

    zox_sys_begin();
    zox_sys_in(Position2);
    zox_sys_in(Rotation2D);
    zox_sys_in(Scale1D);
    zox_sys_in(Brightness);
    zox_sys_in(MaterialGPULink);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position2, position2);
        zox_sys_i(Rotation2D, rotation2D);
        zox_sys_i(Scale1D, scale1D);
        zox_sys_i(Brightness, brightness);
        zox_sys_i(MaterialGPULink, materialGPULink);
        zox_sys_i(TextureGPULink, texture);

        uint material = materialGPULink->value;
        if (!material) {
            zox_log_error("invalid material2D");
            continue;
        }

        MaterialTextured2D attributes = create_MaterialTextured2D(material);

        zox_gpu_material(material);
        zox_gpu_bind_texture(texture->value);

        zox_gpu_bind_buffer_element(global_mesh2.x);
        zox_gpu_bind_buffer_array(global_mesh2.y);
        zox_gpu_enable_attribute_float2(attributes.vertex_position);
        zox_gpu_bind_buffer_array(global_mesh_uvs);
        zox_gpu_enable_attribute_float2(attributes.vertex_uv);

        zox_gpu_float4x4(attributes.camera_matrix, render_camera_matrix);
        zox_gpu_float3(attributes.position, (float3) { position2->value.x, position2->value.y, position_z });
        zox_gpu_float(attributes.angle, rotation2D->value);
        zox_gpu_float(attributes.scale, scale1D->value);
        zox_gpu_float(attributes.brightness, brightness->value);
        zox_gpu_float(attributes.alpha, 1);

        zox_gpu_render(6);

        zox_gpu_bind_texture(0);
        zox_disable_material();
    }

    zox_gpu_disable_blend();
    // zox_gpu_enable_depth_test();
    // zox_gpu_enable_culling();
} zox_sys_end(RenderMaterial2DSystem);
