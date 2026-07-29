static inline float2 get_layout_local_position(
    int2 parent_size,
    int2 layout_position,
    float2 anchor,
    float2 canvas_sizef,
    float aspect_ratio
) {
    int2 p;
    p.x = -(parent_size.x / 2);
    p.y = -(parent_size.y / 2);
    p.x += (int)(parent_size.x * anchor.x);
    p.y += (int)(parent_size.y * anchor.y);
    p.x += layout_position.x;
    p.y += layout_position.y;
    // Convert pixel position to screen
    float2 positionf = int2_to_float2(p);
    positionf = float2_divide(positionf, canvas_sizef);
    positionf.x *= aspect_ratio;
    return positionf;
}

static inline void update_layout_recursive(
    ecs *world,
    entity e,
    int2 parent_size,
    float2 canvas_sizef,
    float aspect_ratio,
    byte dbg_log
) {
    if (!zox_valid(e)) {
        return;
    }
    // LinePosition, float4s
    if (zox_has(e, LayoutLinePoints) &&
        zox_has(e, LineAnchor) &&
        zox_has(e, LocalLinePoints2))
    {
        const LayoutLinePoints* points = zox_get(e, LayoutLinePoints);
        float2 anchor = zox_getv(e, LineAnchor);
        zox_mut_begin(e, LocalLinePoints2, line);
        line->start = get_layout_local_position(
            parent_size,
            points->start,
            anchor,
            canvas_sizef,
            aspect_ratio);
        line->end = get_layout_local_position(
            parent_size,
            points->end,
            anchor,
            canvas_sizef,
            aspect_ratio);
        if (dbg_log) {
            zox_log("[%s] local line [%fx%f] -> [%fx%f]", zox_getn(e), line->start.x, line->start.y, line->end.x, line->end.y);
        }
    }
    if (zox_has(e, LayoutPosition) &&
        zox_has(e, Anchor) &&
        zox_has(e, LocalPosition2))
    {
        int2 layout_position = zox_getv(e, LayoutPosition);
        float2 position_anchor = zox_getv(e, Anchor);
        zox_mut_begin(e, LocalPosition2, local_position);
        local_position->value = get_layout_local_position(
            parent_size,
            layout_position,
            position_anchor,
            canvas_sizef,
            aspect_ratio);
        if (dbg_log) {
            zox_log("[%s] new local position [%fx%f]", zox_getn(e), local_position->value.x, local_position->value.y);
        }
    }
    if (!zox_has(e, LayoutSize)) {
        return;
    }
    int2 size = zox_getv(e, LayoutSize);
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            update_layout_recursive(
                world,
                it.entities[i],
                size,
                canvas_sizef,
                aspect_ratio,
                dbg_log);
        }
    }
}

zox_sys2(LayoutSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSize, canvas_size);
        float2 canvas_sizef = int2_to_float2(canvas_size->value);
        float aspect_ratio = canvas_sizef.x / canvas_sizef.y;
        if (dbg_log) {
            zox_log("[%s] Canvas Size [%ix%i]", zox_getn(e),  canvas_size->value.x, canvas_size->value.y);
        }
        update_layout_recursive(
            world,
            e,
            canvas_size->value,
            canvas_sizef,
            aspect_ratio,
            dbg_log);
    }
}
zox_sys_end(LayoutSystem);
