const float cube_lines_length = 1.0f;

static inline void set_line3D_color(color_rgb value) {
    const float3 valuef3 = color_rgb_to_float3(value);
    zox_gpu_float4(line3D_color_location, (float4) { valuef3.x, valuef3.y, valuef3.z, 1 });
}

static inline void zox_render_line_attr(float3 a, float3 b) {
    // glLineWidth(cubeLinesThickness->value);
    glVertexAttribPointer(line3D_position_location, 3, GL_FLOAT, GL_FALSE, 0,
        (float[]) { a.x, a.y, a.z, b.x, b.y, b.z });
    zox_gpu_render_lines(2);
}

// TODO: Use Instancing here, lags!
zox_sys2(CubeLineRenderSystem) {
    zox_gpu_material(line3D_material);
    glEnableVertexAttribArray(line3D_position_location);
    zox_gpu_float4(line3D_fog_data_location, get_fog_value());
    zox_gpu_float4x4(line3D_camera_matrix_location, render_camera_matrix);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DebugCubeLines);
    zox_sys_in(CubeLinesThickness);
    zox_sys_in(Color);
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    zox_sys_in(Bounds3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DebugCubeLines, dmode);
        zox_sys_i(CubeLinesThickness, thickness);
        zox_sys_i(Color, colorr);
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_i(Bounds3D, bounds);

        const byte mode = dmode->value;
        if (!mode) {
            continue;
        }

        float3 b = bounds->value;
        if (zox_has(e, DebugCubeShrink)) {
            zox_geter_value(e, DebugCubeShrink, float, shrink);
            b = float3_scale(b, shrink);
        }

        float3 p = position->value;
        if (zox_has(e, DebugCubeCorner)) {
            p = float3_add(p, (bounds->value));
        }

        zox_gpu_line_thickness(thickness->value * viewport_scale);
        color_rgb lines_color = color_to_color_rgb(colorr->value);

        if (mode == zox_cubeline_debug_transforms) {
            // up axis
            // zox_render_line_attr(p, (float3) { p.x, p.y + cube_lines_length, p.z });
            set_line3D_color(lines_color);
            float3 up_vector = (float3) { 0, cube_lines_length, 0 };
            float4_rotate_float3_p(rotation->value, &up_vector);
            float3_add_float3_p(&up_vector, p);
            zox_render_line_attr(p, up_vector);
            // forward axis
            set_line3D_color(lines_color);
            float3 forward_vector = (float3) { 0, 0, cube_lines_length };
            float4_rotate_float3_p(rotation->value, &forward_vector);
            float3_add_float3_p(&forward_vector, p);
            zox_render_line_attr(p, forward_vector);
            // right axis
            set_line3D_color(lines_color);
            float3 right_vector = (float3) { cube_lines_length, 0, 0 };
            float4_rotate_float3_p(rotation->value, &right_vector);
            float3_add_float3_p(&right_vector, p);
            zox_render_line_attr(p, right_vector);
        } else {
            // this is normal case
            set_line3D_color(lines_color);
            // get corners of cube
            float3 top_right = (float3) { b.x, b.y, b.z };
            float3 top_left = (float3) { - b.x, b.y, b.z };
            float3 top_right2 = (float3) { b.x, b.y, - b.z };
            float3 top_left2 = (float3) { - b.x, b.y, - b.z };
            float3 bottom_right = (float3) { b.x, - b.y, b.z };
            float3 bottom_left = (float3) { - b.x, - b.y, b.z };
            float3 bottom_right2 = (float3) { b.x, - b.y, - b.z };
            float3 bottom_left2 = (float3) { - b.x, - b.y, - b.z };
            float4_rotate_float3_p(rotation->value, &top_right);
            float4_rotate_float3_p(rotation->value, &top_left);
            float4_rotate_float3_p(rotation->value, &top_right2);
            float4_rotate_float3_p(rotation->value, &top_left2);
            float4_rotate_float3_p(rotation->value, &bottom_right);
            float4_rotate_float3_p(rotation->value, &bottom_left);
            float4_rotate_float3_p(rotation->value, &bottom_right2);
            float4_rotate_float3_p(rotation->value, &bottom_left2);
            float3_add_float3_p(&top_right, p);
            float3_add_float3_p(&top_left, p);
            float3_add_float3_p(&top_right2, p);
            float3_add_float3_p(&top_left2, p);
            float3_add_float3_p(&bottom_right, p);
            float3_add_float3_p(&bottom_left, p);
            float3_add_float3_p(&bottom_right2, p);
            float3_add_float3_p(&bottom_left2, p);
            // top
            zox_render_line_attr(top_left, top_right);
            zox_render_line_attr(top_left2, top_right2);
            zox_render_line_attr(top_left, top_left2);
            zox_render_line_attr(top_right, top_right2);
            // bottom
            zox_render_line_attr(bottom_left, bottom_right);
            zox_render_line_attr(bottom_left2, bottom_right2);
            zox_render_line_attr(bottom_left, bottom_left2);
            zox_render_line_attr(bottom_right, bottom_right2);
            // vertical
            zox_render_line_attr(bottom_right, top_right);
            zox_render_line_attr(bottom_left, top_left);
            zox_render_line_attr(bottom_right2, top_right2);
            zox_render_line_attr(bottom_left2, top_left2);
        }
    }
    zox_gpu_disable_buffer(line3D_position_location);
    zox_disable_material();
} zox_sys_end(CubeLineRenderSystem);