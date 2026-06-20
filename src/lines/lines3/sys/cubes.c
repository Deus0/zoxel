const float cube_lines_length = 1.0f;

// TODO: Use Instancing here, lags!
zox_sys2(CubeLineRenderSystem) {
    zox_gpu_material(line3D_material);
    zox_gpu_enable_attribute(line3D_position_location);
    zox_gpu_float4(line3D_fog_data_location, get_fog_value());
    zox_gpu_float4x4(line3D_camera_matrix_location, render_camera_matrix);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DebugCubeLines);
    zox_sys_in(LineThickness);
    zox_sys_in(Color);
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    zox_sys_in(Bounds3D);
    camera_filtering_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DebugCubeLines, dmode);
        zox_sys_i(LineThickness, thickness);
        zox_sys_i(Color, fill);
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_i(Bounds3D, bounds);
        if (!dmode->value) {
            continue;
        }
        camera_filtering_check();
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
        // color_rgb lines_color = color_to_color_rgb(colorr->value);
        zox_gpu_color(line3D_color_location, fill->value);
        byte depth_test_disabled = zox_has(e, DisableDepthTest) ? 1 : 0;
        if (depth_test_disabled) {
            zox_gpu_disable_depth_test();
        }
        if (fill->value.a != 255) {
            zox_gpu_enable_blend();
        }
        if (dmode->value == zox_cubeline_debug_transforms) {
            // up axis
            // zox_gpu_line(p, (float3) { p.x, p.y + cube_lines_length, p.z });
            // set_line3D_color(lines_color);
            float3 up_vector = (float3) { 0, cube_lines_length, 0 };
            float4_rotate_float3_p(rotation->value, &up_vector);
            float3_add_float3_p(&up_vector, p);
            zox_gpu_line(p, up_vector);
            // forward axis
            // set_line3D_color(lines_color);
            float3 forward_vector = (float3) { 0, 0, cube_lines_length };
            float4_rotate_float3_p(rotation->value, &forward_vector);
            float3_add_float3_p(&forward_vector, p);
            zox_gpu_line(p, forward_vector);
            // right axis
            // set_line3D_color(lines_color);
            float3 right_vector = (float3) { cube_lines_length, 0, 0 };
            float4_rotate_float3_p(rotation->value, &right_vector);
            float3_add_float3_p(&right_vector, p);
            zox_gpu_line(p, right_vector);
        } else {
            // this is normal case
            // set_line3D_color(lines_color);
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
            float extrude = zox_has(e, CubeLineExtrude) ? zox_getv(e, CubeLineExtrude) : 0;
            if (extrude) {
                float shrink = zox_has(e, CubeLineShrink) ? zox_getv(e, CubeLineShrink) : 1;
                // top
                zox_gpu_quad_lines(p, top_left, top_right, top_left2, top_right2, (float3) { shrink, 1, shrink }, (float3) { 0, extrude, 0 });
                // bottom
                zox_gpu_quad_lines(p, bottom_left, bottom_right, bottom_left2, bottom_right2, (float3) { shrink, 1, shrink },  (float3) { 0, -extrude, 0 });
                // back
                zox_gpu_quad_lines(p, top_left2, top_right2, bottom_left2, bottom_right2, (float3) { shrink, shrink, 1 },  (float3) { 0, 0, -extrude });
                // front
                zox_gpu_quad_lines(p, top_left, top_right, bottom_left, bottom_right, (float3) { shrink, shrink, 1 },  (float3) { 0, 0, extrude });
                // left
                zox_gpu_quad_lines(p, top_left, bottom_left, top_left2, bottom_left2, (float3) { 1, shrink, shrink }, (float3) { -extrude, 0, 0 });
                // right
                zox_gpu_quad_lines(p, top_right, bottom_right, top_right2, bottom_right2, (float3) { 1, shrink, shrink }, (float3) { extrude, 0, 0 });
            } else {
                float3_add_float3_p(&top_right, p);
                float3_add_float3_p(&top_left, p);
                float3_add_float3_p(&top_right2, p);
                float3_add_float3_p(&top_left2, p);
                float3_add_float3_p(&bottom_right, p);
                float3_add_float3_p(&bottom_left, p);
                float3_add_float3_p(&bottom_right2, p);
                float3_add_float3_p(&bottom_left2, p);
                // top
                zox_gpu_line(top_left, top_right);
                zox_gpu_line(top_left2, top_right2);
                zox_gpu_line(top_left, top_left2);
                zox_gpu_line(top_right, top_right2);
                // bottom
                zox_gpu_line(bottom_left, bottom_right);
                zox_gpu_line(bottom_left2, bottom_right2);
                zox_gpu_line(bottom_left, bottom_left2);
                zox_gpu_line(bottom_right, bottom_right2);
                // vertical
                zox_gpu_line(bottom_right, top_right);
                zox_gpu_line(bottom_left, top_left);
                zox_gpu_line(bottom_right2, top_right2);
                zox_gpu_line(bottom_left2, top_left2);
            }
        }
        if (fill->value.a != 255) {
            zox_gpu_disable_blend();
        }
        if (depth_test_disabled) {
            zox_gpu_enable_depth_test();
        }
        zox_sys_increment();
    }
    zox_gpu_disable_attribute(line3D_position_location);
    zox_disable_material();
} zox_sys_end(CubeLineRenderSystem);
