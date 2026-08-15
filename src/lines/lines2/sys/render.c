// cache camera?? todo: refactor!
void line2D_render_iteration(iter *it, byte is_element_line) {
    byte dbg_log = 0;
    float depth_per_layer = zox_depth_per_layer;
    float depth_begin = depth_per_layer;
    // gl settings
    zox_gpu_enable_blend();
    if (zox_new_ui_renderer) {
        zox_gpu_enable_depth_test();
    }
    // material
    zox_gpu_material(line2D_material);
    zox_gpu_enable_attribute(line2D_position_location);
    zox_gpu_float4x4(line2D_camera_matrix_location, render_camera_matrix);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LinePoints2);
    zox_sys_in(LineThickness);
    zox_sys_in(Color);
    zox_sys_in(Layer);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Layer, layer);
        zox_sys_i(LinePoints2, line);
        zox_sys_i(LineThickness, thickness);
        zox_sys_i(Color, line_color);
        if (is_element_line) {
            if (!zox_new_ui_renderer && layer->value != renderer_layer) {
                continue; // render per layer
            }
            entity root_camera = zox_get_mesh2_camera(world, e);
            if (root_camera != renderer_camera) {
                continue;
            }
        }
        float depth = depth_begin + layer->value * depth_per_layer;
        zox_gpu_float(line2D_depth_location, depth);
        zox_gpu_line_thickness(thickness->value);
        // zox_gpu_set_attribute_float2(line2D_position_location, &line);
        float values[] = { line->start.x, line->start.y, line->end.x, line->end.y };
        zox_gpu_set_attribute_float2(line2D_position_location, values);
        /*float values[] = {
            line->value.x,
            line->value.y,
            depth,
            line->value.z,
            line->value.w,
            depth
        };
        zox_gpu_set_attribute_float3(line2D_position_location, values);*/
        float4 color_f4 = color_to_float4(line_color->value);
        zox_gpu_float4(line2D_color_location, color_f4);
        zox_gpu_render_lines(2);
        if (dbg_log) {
            zox_log("Rendered Line2 [%s] Layer [%i]", zox_getn(e), layer->value);
        }
        // zox_log("   - [%fx%f] -> [%fx%f]", lineData2D->value.x, lineData2D->value.y, lineData2D->value.z, lineData2D->value.w)
        // zox_log("line_color: %ix%ix%ix%i", line_color->value.r, line_color->value.g, line_color->value.b, line_color->value.a);
        // zox_log("color_f4: %fx%fx%fx%f", color_f4.x, color_f4.y, color_f4.z, color_f4.w);
    }
    zox_gpu_disable_attribute(line2D_position_location);
    zox_disable_material();
    zox_gpu_disable_blend();
    if (zox_new_ui_renderer) {
        zox_gpu_disable_depth_test();
    }
}

zox_sys2(Line2DRenderSystem) {
    line2D_render_iteration(it, 0);
} zox_sys_end(Line2DRenderSystem);

zox_sys2(ElementLine2DRenderSystem) {
    line2D_render_iteration(it, 1);
} zox_sys_end(ElementLine2DRenderSystem);
