// TODO: Make a dirty flag for canvas line points
zox_sys2(Line2DElementSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LinePosition2);
    zox_sys_out(LineData2D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LinePosition2, points);
        zox_sys_o(LineData2D, pointsf);
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        int2 canvas_size_i2 = zox_getv(canvas, LayoutSize);
        const float2 canvas_size = int2_to_float2(canvas_size_i2);
        float aspect_ratio = canvas_size.x / canvas_size.y;
        float2 a = get_element_position((int2) { points->value.x, points->value.y }, canvas_size, aspect_ratio);
        float2 b = get_element_position((int2) { points->value.z, points->value.w }, canvas_size, aspect_ratio);
        pointsf->value = (float4) { a.x, a.y, b.x, b.y };
    }
} zox_sys_end(Line2DElementSystem);
