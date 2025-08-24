// if pixel position moves, update real position
// if it updates, also update any childrens positions too
// has to also move children and their children
byte is_log_dragging = 0;

void ElementDragSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DraggableState);
    zox_sys_in(DraggingDelta);
    zox_sys_in(DraggedLink);

    for (int i = 0; i < it->count; i++) {

        zox_sys_e();
        zox_sys_i(DraggableState, dragableState);
        zox_sys_i(DraggingDelta, delta);
        zox_sys_i(DraggedLink, draggedLink);

        if (!dragableState->value) {
            continue;
        }

        if (delta->value.x != 0 || delta->value.y != 0) {
            entity dragged = draggedLink->value;
            if (!dragged) {
                dragged = e;
            }
            // zox_log("[%s] is now dragging [%s] by ", zox_get_name(e), zox_get_name(dragged))
            // zox_log("> dragging [%s] [%ix%i]", zox_get_name(dragged), draggingDelta->value.x, draggingDelta->value.y)

            // TODO: We should check new position equals before setting
            zox_muter(dragged, LayoutPosition, position);
            zox_muter(dragged, LayoutPositionDirty, dirty);
            int2 new_position = position->value;
            new_position.x += delta->value.x;
            new_position.y += delta->value.y;
            if (zox_has(dragged, LayoutConstraints)) {
                zox_geter_value(dragged, LayoutConstraints, int4, b);
                //if (!int4_equals(b, int4_zero)) {
                limited_element(&new_position, b);
                //}

                if (is_log_dragging) {
                    zox_log("   - bounded by [%s] by x[%i-%i] y[%i-%i]", zox_get_name(dragged), b.x, b.y, b.z, b.w)
                }
            }

            if (!int2_equals(position->value, new_position)) {
                position->value = new_position;
                dirty->value = zox_dirty_trigger;
                if (is_log_dragging) {
                    zox_log("> dragging [%s] by %ix%i", zox_get_name(e), delta->value.x, delta->value.y)
                }
            }
        }
    }
} zoxd_system2(ElementDragSystem);
