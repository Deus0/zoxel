void set_window_bounds_to_canvas(
    ecs *world,
    const entity e,
    const int2 canvas_size,
    const int2 window_size,
    const float2 anchor
) {
    // note: can't actually use components in frame we spawn them
    const float2 anchor_reverse = (float2) { 1 - anchor.x, 1 - anchor.y };
    int4 drag_limits = (int4) {
        - canvas_size.x * anchor.x + window_size.x / 2,
        canvas_size.x * anchor_reverse.x - window_size.x / 2,
        - canvas_size.y * anchor.y + window_size.y / 2,
        canvas_size.y * anchor_reverse.y - window_size.y / 2
    };
    zox_set(e, LayoutConstraints, { drag_limits });
    zox_log("Limiting e [%s] [%ix%ix%ix%i]",
        zox_get_name(e),
        drag_limits.x, drag_limits.y, drag_limits.z, drag_limits.w);
}

void limited_element(
    int2* position,
    const int4 b
) {
    if (position->x < b.x) {
        position->x = b.x;
    }
    if (position->x > b.y) {
        position->x = b.y;
    }
    if (position->y < b.z) {
        position->y = b.z;
    }
    if (position->y > b.w) {
        position->y = b.w;
    }
}

void limit_element(
    ecs *world,
    const entity e
) {
    if (!zox_valid(e) || !zox_has(e, LayoutPosition) || !zox_has(e, LayoutConstraints)) {
        return;
    }
    zox_muter(e, LayoutPosition, pixel_position);
    zox_geter_value(e, LayoutConstraints, int4, drag_bounds);
    limited_element(&pixel_position->value, drag_bounds);
}