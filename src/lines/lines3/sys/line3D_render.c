zox_sys2(Line3DRenderSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LineData3D);
    zox_sys_in(LineThickness);
    zox_sys_in(Color);

    camera_filtering_begin();

    zox_gpu_blend_enable();
    zox_gpu_material(line3D_material);
    zox_gpu_float4(line3D_fog_data_location, get_fog_value());
    zox_gpu_float4x4(line3D_camera_matrix_location, render_camera_matrix);
    glEnableVertexAttribArray(line3D_position_location);

    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LineData3D, data);
        zox_sys_i(LineThickness, thickness);
        zox_sys_i(Color, c);

        camera_filtering_check();

        float3 position_a = (float3) {
            data->value.x,
            data->value.y,
            data->value.z
        };
        float3 position_b = (float3) {
            data->value.w,
            data->value.u,
            data->value.v
        };

        float distance = float3_length(float3_sub(render_camera_position, float3_mid(position_a, position_b)));

        zox_gpu_line_thickness((thickness->value / distance) * viewport_scale);

        float4 colorf = color_to_float4(c->value);

        zox_gpu_set_attribute_float3(line3D_position_location, &data->value);
        // glVertexAttribPointer(line3D_position_location, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat*) &data->value);

        zox_gpu_float4(line3D_color_location, colorf);

        zox_gpu_render_lines(2);
    }

    glDisableVertexAttribArray(line3D_position_location);
    zox_disable_material();
    zox_gpu_blend_disable();

} zox_sys_end(Line3DRenderSystem);
