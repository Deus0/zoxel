
static inline void update_layout3_recursive(
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
    if (zox_has(e, LayoutPosition) &&
        zox_has(e, Anchor) &&
        zox_has(e, LocalPosition3D))
    {
        byte layer = zox_has(e, Layer2D) ? zox_getv(e, Layer2D) : 0;
        int2 layout_position = zox_getv(e, LayoutPosition);
        float2 position_anchor = zox_getv(e, Anchor);
        zox_mut_begin(e, LocalPosition3D, local_position);
        float2 local_position2 = get_layout_local_position(
            parent_size,
            layout_position,
            position_anchor,
            canvas_sizef,
            aspect_ratio);
        local_position->value = (float3) {
            local_position2.x,
            local_position2.y,
            layer * zox_depth_per_layer3
        };
        if (dbg_log) {
            zox_log("[%s] new local position 3 [%fx%f]", zox_getn(e), local_position->value.x, local_position->value.y);
        }
    }
    if (!zox_has(e, LayoutSize)) {
        return;
    }
    int2 size = zox_getv(e, LayoutSize);
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            update_layout3_recursive(
                world,
                it.entities[i],
                size,
                canvas_sizef,
                aspect_ratio,
                dbg_log);
        }
    }
}

zox_sys2(Layout3System) {
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
        update_layout3_recursive(
            world,
            e,
            canvas_size->value,
            canvas_sizef,
            aspect_ratio,
            dbg_log);
    }
}
zox_sys_end(Layout3System);
