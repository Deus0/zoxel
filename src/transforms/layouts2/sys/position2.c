// Convert LayoutPosition to Position2
// Triggered by LayoutPositionDirty

float2 get_element_position(
    const int2 pixel_positionv,
    const int2 canvas_size
) {
    const float2 canvas_size_f = int2_to_float2(canvas_size);
    const float aspect_ratio = canvas_size_f.x / canvas_size_f.y;
    float2 position = int2_to_float2(pixel_positionv);
    float2_divide_float2(&position, canvas_size_f);
    // we get our 0 to 1, make -0.5 to 0.5, then stretch x along canvas
    position.x -= 0.5f;
    position.y -= 0.5f;
    position.x *= aspect_ratio;

    return position;
}

void set_child_position2(
    ecs* world,
    entity e,
    int2 canvas_size
) {
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
            set_child_position2(
                world,
                e2,
                canvas_size
            );
        }
    }
}

void LayoutPosition2System(iter *it) {
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
        zox_sys_o(Position2, position2);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (!zox_valid(canvas->value)) {
            zox_log("! invalid canvas [%s::%lu]",
                zox_get_name(it->entities[i]),
                it->entities[i]);
            continue;
        }
        zox_geter_value(canvas->value, LayoutSize, int2, canvas_size);
        position2->value = get_element_position(
            canvas_position->value,
            canvas_size
        );

        zox_sys_e();
        set_child_position2(world, e, canvas_size);

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
} zoxd_system2(LayoutPosition2System);