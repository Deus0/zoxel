// if pixel position moves, update real position
// if it updates, also update any childrens positions too
// has to also move children and their children

void element_drag_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0 || is_log_dragging;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DraggableState);
    zox_sys_in(DraggingDelta);
    //zox_sys_in(DraggedLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DraggableState, state);
        zox_sys_i(DraggingDelta, delta);
        //zox_sys_i(DraggedLink, dragged);
        if (!state->value) {
            continue;
        }
        if (!delta->value.x && !delta->value.y) {
            continue;
        }
        entity dragged = zox_get_link(world, e, DraggedLink);
        if (zox_has(e, DragResizes)) {
            if (!zox_valid(dragged) || !zox_has(dragged, LayoutSize)) {
                continue;
            }
            const float2 anchor = zox_getv(e, Anchor);
            const int2 before = zox_getv(dragged, LayoutSize);
            // a side changes one axis, a corner both; the opposite edge stays (the centre moves half the change)
            int2 size = before;
            int2 position = zox_getv(dragged, LayoutPosition);
            if (anchor.x == 0 || anchor.x == 1) {
                const int sign = anchor.x == 0 ? -1 : 1;
                size.x = int_max(120, before.x + sign * delta->value.x);
                position.x += sign * (size.x - before.x) / 2;
            }
            if (anchor.y == 0 || anchor.y == 1) {
                const int sign = anchor.y == 0 ? -1 : 1;
                size.y = int_max(80, before.y + sign * delta->value.y);
                position.y += sign * (size.y - before.y) / 2;
            }
            // no larger than its canvas (window_canvas_fit keeps it on it)
            const entity canvas = zox_get_parent_by_id(world, dragged, zox_id(Canvas));
            if (zox_valid(canvas)) {
                const int2 canvas_size = zox_getv(canvas, LayoutSize);
                const int2 fit = { int_min(size.x, int_max(before.x, canvas_size.x)), int_min(size.y, int_max(before.y, canvas_size.y)) };
                position.x -= (anchor.x == 0 ? -1 : anchor.x == 1 ? 1 : 0) * (size.x - fit.x) / 2;
                position.y -= (anchor.y == 0 ? -1 : anchor.y == 1 ? 1 : 0) * (size.y - fit.y) / 2;
                size = fit;
            }
            if (!int2_equals(size, before)) {
                zox_setv(dragged, LayoutSize, size);
                zox_setv(dragged, LayoutPosition, position);
                zox_add(dragged, Dirty);
            }
            continue;
        }
        entity e2 = !dragged ? e : dragged;
        // TODO: We should check new position equals before setting
        zox_muter(e2, LayoutPosition, position);
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
            if (dbg_log) {
                zox_log("> dragging [%s] by %ix%i", zox_getn(e), delta->value.x, delta->value.y);
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(element_drag_system);
