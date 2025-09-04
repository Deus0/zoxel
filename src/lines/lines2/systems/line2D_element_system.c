float2 get_ui_real_position2_canvas_no_anchor(
    const int2 local_pixel_position,
    const float2 canvas_size_f// ,
    // const float aspect_ratio
) {
    return (float2) { (2.0f * (local_pixel_position.x  / canvas_size_f.x) - 1.0f), // aspect_ratio,
            (2.0f * (local_pixel_position.y  / canvas_size_f.y) - 1.0f) };
}

void set_ui_line_position(
    LineData2D *pointsf,
    const int4 points,
    const float2 canvas_size_f
) {
    const float2 point_a = get_ui_real_position2_canvas_no_anchor(
        (int2) { points.x, points.y },
        canvas_size_f
    );
    const float2 point_b = get_ui_real_position2_canvas_no_anchor(
        (int2) { points.z, points.w },
        canvas_size_f
    );
    pointsf->value = (float4) { point_a.x, point_a.y, point_b.x, point_b.y };
}

// this just sets posiions - sets render position based on canvas

// TODO: Make a dirty flag for canvas line points
void Line2DElementSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LinePosition2);
    zox_sys_in(CanvasLink);
    zox_sys_out(LineData2D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CanvasLink, canvas);
        zox_sys_i(LinePosition2, points);
        zox_sys_o(LineData2D, data);

        if (!zox_valid(canvas->value)) {
            continue;
        }

        zox_geter_value(canvas->value, LayoutSize, int2, canvas_size);
        const float2 canvas_size_f = int2_to_float2(canvas_size);

        set_ui_line_position(
            data,
            points->value,
            canvas_size_f
        );
    }
} zoxd_system2(Line2DElementSystem);