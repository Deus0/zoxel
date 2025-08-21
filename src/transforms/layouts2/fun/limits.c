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
    zox_set(e, DraggableLimits, { drag_limits });
}

void limited_element(
    PixelPosition *pixel_position,
    const int4 drag_bounds
) {
    if (pixel_position->value.x < drag_bounds.x) {
        pixel_position->value.x = drag_bounds.x;
    }
    if (pixel_position->value.x > drag_bounds.y) {
        pixel_position->value.x = drag_bounds.y;
    }
    if (pixel_position->value.y < drag_bounds.z) {
        pixel_position->value.y = drag_bounds.z;
    }
    if (pixel_position->value.y > drag_bounds.w) {
        pixel_position->value.y = drag_bounds.w;
    }
}

void limit_element(
    ecs *world,
    const entity e
) {
    if (!zox_valid(e) || !zox_has(e, PixelPosition) || !zox_has(e, DraggableLimits)) {
        return;
    }
    zox_muter(e, PixelPosition, pixel_position);
    zox_geter_value(e, DraggableLimits, int4, drag_bounds);
    limited_element(pixel_position, drag_bounds);
}