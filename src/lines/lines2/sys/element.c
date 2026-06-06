float2 get_ui_real_position2_canvas_no_anchor(int2 local_pixel_position, float2 canvas_size_f) {
    return (float2) { (2.0f * (local_pixel_position.x  / canvas_size_f.x) - 1.0f), // aspect_ratio,
            (2.0f * (local_pixel_position.y  / canvas_size_f.y) - 1.0f) };
}

void set_ui_line_position(LineData2D *pointsf, int4 points, float2 canvas_size_f) {
    float2 point_a = get_ui_real_position2_canvas_no_anchor(
        (int2) { points.x, points.y },
        canvas_size_f
    );
    float2 point_b = get_ui_real_position2_canvas_no_anchor(
        (int2) { points.z, points.w },
        canvas_size_f
    );
    pointsf->value = (float4) { point_a.x, point_a.y, point_b.x, point_b.y };
}

// this just sets posiions - sets render position based on canvas

// TODO: Make a dirty flag for canvas line points
zox_sys2(Line2DElementSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LinePosition2);
    zox_sys_out(LineData2D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LinePosition2, points);
        zox_sys_o(LineData2D, data);
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        zox_geter_value(canvas, LayoutSize, int2, canvas_size);
        const float2 canvas_size_f = int2_to_float2(canvas_size);
        set_ui_line_position(data, points->value, canvas_size_f);
    }
} zox_sys_end(Line2DElementSystem);
