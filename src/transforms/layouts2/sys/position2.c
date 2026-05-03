// Convert LayoutPosition to Position2
// Triggered by LayoutPositionDirty

float2 get_element_position(int2 position, int2 size) {

    float2 sizef = int2_to_float2(size);
    float aspect_ratio = sizef.x / sizef.y;
    float2 positionf = int2_to_float2(position);
    float2_divide_float2(&positionf, sizef);
    // we get our 0 to 1, make -0.5 to 0.5, then stretch x along canvas
    positionf.x -= 0.5f;
    positionf.y -= 0.5f;
    positionf.x *= aspect_ratio;

    return positionf;
}

void set_layout_child_position_recursively(ecs* world, entity e, int2 canvas_size) {

    if (!zox_valid(e)) {
        return;
    }

    if (zox_has(e, CanvasPosition) && zox_has(e, Position2)) {
        zox_geter_value(e, CanvasPosition, int2, canvas_position);
        zox_muter(e, Position2, position2);
        position2->value = get_element_position(
            canvas_position,
            canvas_size
        );
    }

    // also set children ones
    if (zox_has(e, Children)) {
        zox_geter(e, Children, children);
        for (int i = 0; i < children->length; i++) {
            entity e2 = children->value[i];

            set_layout_child_position_recursively(world, e2, canvas_size);
        }
    }
}

// Should this just include canvas too?
zox_sys2(LayoutPosition2System) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutPositionDirty);
    zox_sys_in(CanvasPosition);
    zox_sys_in(CanvasLink);
    zox_sys_out(Position2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LayoutPositionDirty, dirty);
        zox_sys_i(CanvasPosition, canvas_position);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_o(Position2, position);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (!zox_valid(canvas->value)) {
            zox_sys_e();
            zox_logw("Element [%s:%lu] has Invalid Canvas: %lu", zox_get_name(e), e, canvas->value);
            continue;
        }

        zox_geter_value(canvas->value, LayoutSize, int2, canvas_size);
        position->value = get_element_position(canvas_position->value, canvas_size);

        zox_sys_e();
        set_layout_child_position_recursively(world, e, canvas_size);

        /*zox_log("[%s] posf [%.1fx%.1f] canvaspos [%ix%i] - canvas size [%ix%i]",
            zox_get_name(it->entities[i]),
            position2->value.x,
            position2->value.y,
            canvas_position->value.x,
            canvas_position->value.y,
            canvas_size.x,
            canvas_size.y
        );*/
    }
} zox_sys_end(LayoutPosition2System);
