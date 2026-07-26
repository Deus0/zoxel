/*void set_line2_canvas_position_direct(ecs *world, entity e, int2 parent_position, int2 parent_size) {
    if (!zox_valid(e)) {
        return;
    }
    zox_geter_value(e, LineAnchor, float4, anchor);
    zox_geter_value(e, LineLocalPosition2, int4, points);
    zox_muter(e, LinePosition2, canvas_points);
    // anchoring - point starts from bottom left, then we add 0 to 1 of parent size
    canvas_points->value = (int4) {
        parent_position.x + points.x + parent_size.x * anchor.x - parent_size.x / 2,
        parent_position.y + points.y + parent_size.y * anchor.y - parent_size.y / 2,
        parent_position.x + points.z + parent_size.x * anchor.z - parent_size.x / 2,
        parent_position.y + points.w + parent_size.y * anchor.w - parent_size.y / 2
    };
}

void set_line2_canvas_position(ecs *world, entity e) {
    if (!zox_valid(e) || !zox_has(e, LineLocalPosition2)) {
        return;
    }
    entity parent = zox_get_parent(world, e);
    if (!zox_valid(parent)) {
        return;
    }
    zox_geter_value(parent, CanvasPosition, int2, parent_position);
    zox_geter_value(parent, LayoutSize, int2, parent_size);
    set_line2_canvas_position_direct(world, e, parent_position, parent_size);
}

// TODO: LinePosition2Dirty
zox_sys2(Line2PositionSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LineLocalPositionDirty);
    zox_sys_in(LineAnchor);
    zox_sys_in(LineLocalPosition2);
    zox_sys_out(LinePosition2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LineLocalPositionDirty, state);
        zox_sys_i(LineAnchor, anchor);
        zox_sys_i(LineLocalPosition2, points);
        zox_sys_o(LinePosition2, canvas_points);
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity parent = zox_get_parent(world, e);
        if (!zox_valid(parent)) {
            continue;
        }
        if (!zox_has(parent, CanvasPosition) || !zox_has(parent, LayoutSize)) {
            continue;
        }
        int2 parent_position = zox_getv(parent, CanvasPosition);
        int2 parent_size = zox_getv(parent, LayoutSize);
        // anchoring - point starts from bottom left, then we add 0 to 1 of parent size
        canvas_points->value = (int4) {
            parent_position.x + points->value.x + parent_size.x * anchor->value.x - parent_size.x / 2,
            parent_position.y + points->value.y + parent_size.y * anchor->value.y - parent_size.y / 2,
            parent_position.x + points->value.z + parent_size.x * anchor->value.z - parent_size.x / 2,
            parent_position.y + points->value.w + parent_size.y * anchor->value.w - parent_size.y / 2
        };
    }
} zox_sys_end(Line2PositionSystem);
*/
        /*iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (!zox_valid(e2) || !zox_has(e2, LineLocalPosition2)) {
                    continue;
                }
                set_line2_canvas_position_direct(world, e2, canvas_position->value, size->value);
            }
        }*/
