zox_sys2(QuadLineRenderSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LineThickness);
    zox_sys_in(Color);
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    zox_sys_in(QuadLineSize);
    camera_filtering_begin();
    zox_gpu_material(line3D_material);
    zox_gpu_enable_attribute(line3D_position_location);
    zox_gpu_float4(line3D_fog_data_location, get_fog_value());
    zox_gpu_float4x4(line3D_camera_matrix_location, render_camera_matrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LineThickness, thickness);
        zox_sys_i(Color, fill);
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation3D);
        zox_sys_i(QuadLineSize, size);
        camera_filtering_check();
        float3 b = (float3) { size->value, size->value, size->value };
        float3 p = position->value;
        zox_gpu_line_thickness(thickness->value * get_viewport_scale());
        // color_rgb lines_color = color_to_color_rgb(colorr->value);
        zox_gpu_color(line3D_color_location, fill->value);
        // get corners of cube
        float3 top_right = (float3) { b.x, b.y, b.z };
        float3 top_left = (float3) { - b.x, b.y, b.z };
        float3 top_right2 = (float3) { b.x, b.y, - b.z };
        float3 top_left2 = (float3) { - b.x, b.y, - b.z };
        float4_rotate_float3_p(rotation3D->value, &top_right);
        float4_rotate_float3_p(rotation3D->value, &top_left);
        float4_rotate_float3_p(rotation3D->value, &top_right2);
        float4_rotate_float3_p(rotation3D->value, &top_left2);
        float3_add_float3_p(&top_right, p);
        float3_add_float3_p(&top_left, p);
        float3_add_float3_p(&top_right2, p);
        float3_add_float3_p(&top_left2, p);
        // top
        zox_gpu_line(top_left, top_right);
        zox_gpu_line(top_left2, top_right2);
        zox_gpu_line(top_left, top_left2);
        zox_gpu_line(top_right, top_right2);
    }
    zox_gpu_disable_attribute(line3D_position_location);
    zox_disable_material();
} zox_sys_end(QuadLineRenderSystem);
