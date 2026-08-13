// if pixel position moves, update real position
// if it updates, also update any childrens positions too
// has to also move children and their children

zox_sys2(ElementDragSystem) {
    byte dbg_log = 0 || is_log_dragging;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DraggableState);
    zox_sys_in(DraggingDelta);
    zox_sys_in(DraggedLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DraggableState, state);
        zox_sys_i(DraggingDelta, delta);
        zox_sys_i(DraggedLink, dragged);
        if (!state->value) {
            continue;
        }
        if (!delta->value.x && !delta->value.y) {
            continue;
        }
        entity e2 = !dragged->value ? e : dragged->value;
        // TODO: We should check new position equals before setting
        zox_muter(e2, LayoutPosition, position);
        zox_muter(e2, LayoutPositionDirty, dirty);
        int2 new_position = position->value;
        new_position.x += delta->value.x;
        new_position.y += delta->value.y;
        // Limits position
        if (zox_has(e2, BoundToParent)) {
            entity dragged_parent = zox_get_parent(world, e2);
            int2 size = zox_getv(e2, LayoutSize);
            float2 anchor = zox_getv(e2, Anchor);
            int2 parent_size = zox_getv(dragged_parent, LayoutSize);
            float2 value_reverse = (float2) { 1 - anchor.x, 1 - anchor.y };
            int4 constraints =  (int4) {
                - parent_size.x * anchor.x + size.x / 2,
                parent_size.x * value_reverse.x - size.x / 2,
                - parent_size.y * anchor.y + size.y / 2,
                parent_size.y * value_reverse.y - size.y / 2
            };
            bound_position2(&new_position, constraints);
            if (dbg_log) {
                zox_log("[%s] is Parent Bound [%s] by x[%i-%i] y[%i-%i]", zox_getn(e2), zox_getn(dragged_parent), constraints.x, constraints.y, constraints.z, constraints.w);
            }
        }
        if (zox_has(e2, LayoutConstraints)) {
            int4 constraints = zox_getv(e2, LayoutConstraints);
            bound_position2(&new_position, constraints);
            if (dbg_log) {
                zox_log("   - bounded by [%s] by x[%i-%i] y[%i-%i]", zox_getn(e2), constraints.x, constraints.y, constraints.z, constraints.w);
            }
        }
        if (!int2_equals(position->value, new_position)) {
            position->value = new_position;
            dirty->value = zox_dirty_trigger;
            if (dbg_log) {
                zox_log("> dragging [%s] by %ix%i", zox_getn(e), delta->value.x, delta->value.y);
            }
        }
    }
} zox_sys_end(ElementDragSystem);
