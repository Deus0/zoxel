void set_line2_canvas_position_direct(ecs *world, entity e, int2 parent_position, int2 parent_size) {
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

// TODO: LinePosition2Dirty -> Reposition there
zox_sys2(Line2PositionSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutPositionDirty);
    zox_sys_in(CanvasPosition);
    zox_sys_in(LayoutSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutPositionDirty, state);
        zox_sys_i(CanvasPosition, cposition);
        zox_sys_i(LayoutSize, size);
        if (state->value != zox_dirty_active) {
            // continue;
        }
        entity children[layouts2_children_capacity];
        uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
        for (uint j = 0; j < children_length; j++) {
            entity e2 = children[j];
            if (!zox_valid(e2) || !zox_has(e2, LineLocalPosition2)) {
                continue;
            }
            set_line2_canvas_position_direct(world, e2, cposition->value, size->value);
        }
    }
} zox_sys_end(Line2PositionSystem);
