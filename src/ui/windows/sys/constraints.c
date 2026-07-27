// NOTE: Sets window cconstraints on init
zox_sys2(CanvasBoundsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Initialize);
    zox_sys_in(LayoutSize);
    zox_sys_in(Anchor);
    zox_sys_out(LayoutConstraints);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Initialize, initialize);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(Anchor, anchor);
        zox_sys_o(LayoutConstraints, constraints);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            zox_loge("Invalid Canvas for Window %s", zox_get_name(e));
            continue;
        }
        // set_window_bounds_to_canvas(world, e, zox_getv(canvas, LayoutSize), size, anchor->value);
        int2 canvas_size = zox_getv(canvas, LayoutSize);
        float2 value_reverse = (float2) { 1 - anchor->value.x, 1 - anchor->value.y };
        constraints->value = (int4) {
            - canvas_size.x * anchor->value.x + size->value.x / 2,
            canvas_size.x * value_reverse.x - size->value.x / 2,
            - canvas_size.y * anchor->value.y + size->value.y / 2,
            canvas_size.y * value_reverse.y - size->value.y / 2
        };
    }
} zox_sys_end(CanvasBoundsSystem);
