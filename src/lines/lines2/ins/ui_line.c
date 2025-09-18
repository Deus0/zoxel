void offset_line_points(int4 *points, const float4 line_anchor, const float2 canvas_size_f) {
    points->x += canvas_size_f.x * line_anchor.x;
    points->y += canvas_size_f.y * line_anchor.y;
    points->z += canvas_size_f.x * line_anchor.z;
    points->w += canvas_size_f.y * line_anchor.w;
}

entity spawn_ui_line2(
    ecs *world,
    entity canvas,
    const entity parent,
    int2 point_a,
    const int2 point_b,
    const float2 anchor_a,
    const float2 anchor_b,
    const color line_color,
    const float thickness,
    const double life_time,
    const float2 parent_positionf,
    const int2 parent_position,
    const byte layer
) {
    if (canvas == 0) {
        canvas = zox_canvases[0];
    }
    // const int2 canvas_size = zox_get_value(canvas, LayoutSize)
    const entity e = life_time ? zox_instancee(prefab_temporary_ui_line2D) : zox_instancee(prefab_ui_line2D);
    zox_name("ui_line2D");
    zox_set(e, CanvasLink, { canvas });
    // const float2 canvas_size_f = { (float) canvas_size.x, (float) canvas_size.y };
    // const float aspect_ratio = canvas_size_f.x / canvas_size_f.y;
    const float4 line_anchor = (float4) {
        anchor_a.x,
        anchor_a.y,
        anchor_b.x,
        anchor_b.y
    };
    int4 points = (int4) {
        point_a.x,
        point_a.y,
        point_b.x,
        point_b.y
    };
    zox_set(e, LineLocalPosition2, { points });
    zox_set(e, Layer2D, { layer });
    zox_set(e, Color, { line_color });
    zox_set(e, LineThickness, { thickness });
    zox_set(e, LineAnchor, { line_anchor });
    if (life_time) {
        zox_set(e, DestroyInTime, { life_time });
    }

    // adds to canvas
    if (parent == canvas) {
        on_child_added(world, canvas, e);
    }

    return e;
}

entity spawn_ui_line2_v2(ecs *world,
    const entity canvas,
    const entity parent,
    const int2 point_a,
    const int2 point_b,
    const color line_color,
    const float thickness,
    const double life_time,
    const float2 parent_positionf,
    const int2 parent_position,
    const byte layer
) {
    return spawn_ui_line2(
        world,
        canvas,
        parent,
        point_a,
        point_b,
        float2_zero,
        float2_zero,
        line_color,
        thickness,
        life_time,
        parent_positionf,
        parent_position,
        layer);
}