// 3D lines: instanced like particles, every line in one draw (line3D_instanced.vert)
//  - each line is an instance, its start and end (LineData3D) and Color are per instance buffers,
//    uploaded as they are in the table (copied first only when a camera filters lines)
//  - one width a draw (gl line width isn't per line): the average of thickness / distance, i.e. gizmos
//    sharing a thickness look the same as before
//  - falls back to a draw a line without instancing (zox_disable_instancing) or if its shader failed
//  - line3D_drawn and line3D_draw_calls count this frame's, for tests (tst lines_render)
uint line3D_drawn = 0;
uint line3D_draw_calls = 0;
uint line3D_counted_frame = UINT32_MAX;
LineData3D* line3D_filtered_data = NULL;
Color* line3D_filtered_colors = NULL;
int line3D_filtered_capacity = 0;

static inline void line3D_count(uint lines, uint draws) {
    if (line3D_counted_frame != ecs_run_count) {
        line3D_counted_frame = ecs_run_count;
        line3D_drawn = 0;
        line3D_draw_calls = 0;
    }
    line3D_drawn += lines;
    line3D_draw_calls += draws;
}

void line3d_render_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LineData3D);
    zox_sys_in(LineThickness);
    zox_sys_in(Color);
    camera_filtering_begin();
    zox_gpu_enable_blend();
#if !defined(zox_disable_instancing)
    const MaterialLine3DInstanced* instanced = zox_valid(material_line3D_instanced) ? zox_get(material_line3D_instanced, MaterialLine3DInstanced) : NULL;
    if (instanced && line3D_instanced_t_buffer) {
        // the lines this camera draws: all, or copied when it filters them
        const LineData3D* data = LineData3D_;
        const Color* colors = Color_;
        int count = it->count;
        if (filtering) {
            if (line3D_filtered_capacity < it->count) {
                line3D_filtered_capacity = it->count * 2;
                line3D_filtered_data = realloc(line3D_filtered_data, sizeof(LineData3D) * line3D_filtered_capacity);
                line3D_filtered_colors = realloc(line3D_filtered_colors, sizeof(Color) * line3D_filtered_capacity);
            }
            count = 0;
            for (int i = 0; i < it->count; i++) {
                camera_filtering_check();
                line3D_filtered_data[count] = LineData3D_[i];
                line3D_filtered_colors[count] = Color_[i];
                count++;
            }
            data = line3D_filtered_data;
            colors = line3D_filtered_colors;
        }
        if (count) {
            // one width, the average of each line's thickness / distance
            float width = 0;
            for (int i = 0; i < it->count; i++) {
                zox_sys_i(LineData3D, line);
                zox_sys_i(LineThickness, thickness);
                const float3 a = (float3) { line->value.x, line->value.y, line->value.z };
                const float3 b = (float3) { line->value.w, line->value.u, line->value.v };
                const float distance = float3_length(float3_sub(render_camera_position, float3_mid(a, b)));
                width += thickness->value / (distance > 0.01f ? distance : 0.01f);
            }
            zox_gpu_line_thickness((width / it->count) * get_viewport_scale());
            zox_gpu_material(zox_getv(material_line3D_instanced, MaterialGPULink));
            zox_gpu_float4x4(instanced->camera_matrix, render_camera_matrix);
            line3D_instanced_reserve(count);
            // per vertex: 0 or 1 along the line
            zox_gpu_bind_buffer_array(line3D_instanced_t_buffer);
            zox_gpu_set_attribute_floatx(instanced->t, (void*) 0, 1);
            zox_gpu_enable_attribute(instanced->t);
            // per line: start and end, interleaved in LineData3D
            zox_gpu_bind_buffer_array(line3D_instanced_data_buffer);
            zox_gpu_set_sub_buffer_bytes(sizeof(LineData3D) * count, data);
            zox_gpu_set_attribute_float3_strided(instanced->start, sizeof(LineData3D), 0);
            zox_gpu_set_attribute_float3_strided(instanced->end, sizeof(LineData3D), sizeof(float3));
            zox_gpu_enable_attribute(instanced->start);
            zox_gpu_enable_attribute(instanced->end);
            zox_gpu_attribute_divisor(instanced->start, 1);
            zox_gpu_attribute_divisor(instanced->end, 1);
            // per line: color
            zox_gpu_bind_buffer_array(line3D_instanced_color_buffer);
            zox_gpu_set_sub_buffer_bytes(sizeof(Color) * count, colors);
            zox_gpu_set_attribute_color(instanced->color, (void*) 0);
            zox_gpu_enable_attribute(instanced->color);
            zox_gpu_attribute_divisor(instanced->color, 1);
            zox_gpu_render_lines_instanced(count);
            // resets
            zox_gpu_attribute_divisor(instanced->color, 0);
            zox_gpu_attribute_divisor(instanced->end, 0);
            zox_gpu_attribute_divisor(instanced->start, 0);
            zox_gpu_disable_attribute(instanced->color);
            zox_gpu_disable_attribute(instanced->end);
            zox_gpu_disable_attribute(instanced->start);
            zox_gpu_disable_attribute(instanced->t);
            zox_gpu_bind_buffer_array(0);
            zox_disable_material();
            line3D_count(count, 1);
        }
        zox_gpu_disable_blend();
        zox_sys_on_end();
        return;
    }
#endif
    // a draw a line
    const MaterialLine3D* line3D = line3D_material_begin(world);
    if (!line3D) {
        zox_gpu_disable_blend();
        zox_sys_on_end();
        return;
    }
    uint drawn = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LineData3D, data);
        zox_sys_i(LineThickness, thickness);
        zox_sys_i(Color, fill);
        camera_filtering_check();
        float3 position_a = (float3) { data->value.x, data->value.y,  data->value.z };
        float3 position_b = (float3) { data->value.w, data->value.u,  data->value.v };
        float distance = float3_length(float3_sub(render_camera_position, float3_mid(position_a, position_b)));
        zox_gpu_line_thickness((thickness->value / distance) * get_viewport_scale());
        zox_gpu_set_attribute_float3(line3D->position, &data->value);
        zox_gpu_color(line3D->color, fill->value);
        zox_gpu_render_lines(2);
        drawn++;
    }
    zox_gpu_disable_attribute(line3D->position);
    zox_disable_material();
    zox_gpu_disable_blend();
    line3D_count(drawn, drawn);
    zox_sys_on_end();
} zoxd_system(line3d_render_system);
