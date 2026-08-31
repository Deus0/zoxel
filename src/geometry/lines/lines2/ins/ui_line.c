void offset_line_points(int4 *points, float4 line_anchor, float2 canvas_size_f) {
    points->x += canvas_size_f.x * line_anchor.x;
    points->y += canvas_size_f.y * line_anchor.y;
    points->z += canvas_size_f.x * line_anchor.z;
    points->w += canvas_size_f.y * line_anchor.w;
}

entity spawn_ui_line2(
    ecs *world,
    entity parent,
    int2 point_a,
    int2 point_b,
    float2 anchor_a,
    float2 anchor_b,
    color line_color,
    float thickness,
    double life_time,
    float2 parent_positionf,
    int2 parent_position,
    byte layer
) {
    // const int2 canvas_size = zox_getv(canvas, LayoutSize)
    entity e = life_time ? zox_ins(world, prefab_temporary_ui_line2D) : zox_ins(world, prefab_ui_line2D);
    zox_name("ui_line2D");
    zox_set_parent(world, e, parent);
    zox_set(e, LayoutLinePoints, { point_a, point_b });
    zox_set(e, Layer, { layer });
    zox_set(e, Color, { line_color });
    zox_set(e, LineThickness, { thickness });
    zox_set(e, LineAnchor, { anchor_a });
    if (life_time) {
        zox_set(e, DestroyInTime, { life_time });
    }
    return e;
}

entity spawn_ui_line2_v2(ecs *world,
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
        layer
    );
}
