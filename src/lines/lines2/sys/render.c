// cache camera?? todo: refactor!
// extern entity zox_get_root_canvas_camera(ecs*, entity);

void line2D_render_iteration(iter *it, byte is_element_line) {
    zox_gpu_material(line2D_material);
    zox_gpu_enable_attribute(line2D_position_location);
    float4x4 render_matrix;
    if (is_element_line) {
        zox_gpu_float(line2D_depth_location, ((int) renderer_layer) * shader_depth_multiplier);
        render_matrix = float4x4_identity;
    } else {
        render_matrix = render_camera_matrix;
        zox_gpu_float(line2D_depth_location, 0);
    }
    zox_gpu_float4x4(line2D_camera_matrix_location, render_matrix);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LineData2D);
    zox_sys_in(LineThickness);
    zox_sys_in(Color);
    zox_sys_in(Layer2D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Layer2D, layer2D);
        zox_sys_i(LineData2D, lineData2D);
        zox_sys_i(LineThickness, lineThickness);
        zox_sys_i(Color, line_color);

        if (is_element_line) {

            if (layer2D->value != renderer_layer) {
                continue; // render per layer
            }

            if (zox_get_root_canvas_camera(world, e) != renderer_camera) {
                continue;
            }
        }
        zox_gpu_line_thickness(lineThickness->value);
        zox_gpu_set_attribute_float2(line2D_position_location, &lineData2D->value);
        // glVertexAttribPointer(line2D_position_location, 2, GL_FLOAT, GL_FALSE, 0, (GLfloat*) &lineData2D->value);
        float4 color_f4 = color_to_float4(line_color->value);
        zox_gpu_float4(line2D_color_location, color_f4);
        zox_gpu_render_lines(2);
        // zox_log("   - [%fx%f] -> [%fx%f]", lineData2D->value.x, lineData2D->value.y, lineData2D->value.z, lineData2D->value.w)
        // zox_log("line_color: %ix%ix%ix%i", line_color->value.r, line_color->value.g, line_color->value.b, line_color->value.a);
        // zox_log("color_f4: %fx%fx%fx%f", color_f4.x, color_f4.y, color_f4.z, color_f4.w);
    }
    zox_gpu_disable_attribute(line2D_position_location);
    zox_disable_material();
}

zox_sys2(Line2DRenderSystem) {
    line2D_render_iteration(it, 0);
} zox_sys_end(Line2DRenderSystem);

zox_sys2(ElementLine2DRenderSystem) {
    line2D_render_iteration(it, 1);
} zox_sys_end(ElementLine2DRenderSystem);
