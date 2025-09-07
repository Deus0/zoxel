/*void resize_ui_line2D(ecs *world, entity e, int2 canvas_size) {
    if (!zox_has(e, LineLocalPosition2)) {
        return;
    }

    const float2 canvas_size_f = { (float) canvas_size.x, (float) canvas_size.y };
    const float aspect_ratio = canvas_size_f.x / canvas_size_f.y;
    zox_geter_value(e, LineAnchor, float4, anchor);
    zox_geter_value(e, LineLocalPosition2, int4, points);

    // todo: fix this, these are parent - canvas atm
    const float2 parent_real_position = (float2) {
        - aspect_ratio / 2.0f,
        - 1 / 2.0f
    };
    const int2 parent_position = int2_zero;
    offset_line_points(&points, anchor, canvas_size_f);
    const int4 line_position2 = get_new_line_position(parent_real_position, canvas_size_f, aspect_ratio, parent_position, points);

    zox_set(e, LinePosition2, { line_position2 });

    zox_muter(e, LineData2D, data)
    set_ui_line_position(data, line_position2, canvas_size_f);
}*/