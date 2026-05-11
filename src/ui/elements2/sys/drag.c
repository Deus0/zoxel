// if pixel position moves, update real position
// if it updates, also update any childrens positions too
// has to also move children and their children

zox_sys2(ElementDragSystem) {
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
        if (zox_has(e2, LayoutConstraints)) {
            zox_geter_value(e2, LayoutConstraints, int4, b);
            limited_element(&new_position, b);

            if (is_log_dragging) {
                zox_log("   - bounded by [%s] by x[%i-%i] y[%i-%i]", zox_get_name(e2), b.x, b.y, b.z, b.w)
            }
        }
        if (!int2_equals(position->value, new_position)) {
            position->value = new_position;
            dirty->value = zox_dirty_trigger;
            if (is_log_dragging) {
                zox_log("> dragging [%s] by %ix%i", zox_get_name(e), delta->value.x, delta->value.y);
            }
        }
    }
} zox_sys_end(ElementDragSystem);
