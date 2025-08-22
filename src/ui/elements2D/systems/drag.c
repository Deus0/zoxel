// if pixel position moves, update real position
// if it updates, also update any childrens positions too
// has to also move children and their children
byte is_log_dragging = 0;

void drag_element(ecs *world, const entity e, const int2 drag_value) {
    if (!zox_valid(e)) {
        return;
    }
}

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
            entity dragged_entity = draggedLink->value;
            if (!dragged_entity) {
                dragged_entity = e;
            }
            // zox_log("[%s] is now dragging [%s] by ", zox_get_name(e), zox_get_name(dragged_entity))
            // zox_log("> dragging [%s] [%ix%i]", zox_get_name(dragged_entity), draggingDelta->value.x, draggingDelta->value.y)

            // drag_element(world, dragged_entity, delta->value);

            // TODO: We should check new position equals before setting
            zox_muter(dragged_entity, PixelPosition, pixel_position);
            zox_muter(dragged_entity, LayoutPositionDirty, dirty);
            pixel_position->value.x += delta->value.x;
            pixel_position->value.y += delta->value.y;
            dirty->value = zox_dirty_trigger;

            if (is_log_dragging) {
                zox_log("> dragging [%s] by %ix%i", zox_get_name(e), delta->value.x, delta->value.y)
            }
            if (zox_has(dragged_entity, DraggableLimits)) {
                zox_geter_value(dragged_entity, DraggableLimits, int4, drag_bounds);
                if (int4_equals(drag_bounds, int4_zero)) {
                    zox_logw("drag bounds [%s] [int4_zero]", zox_get_name(dragged_entity));
                }
                limited_element(pixel_position, drag_bounds);
                if (is_log_dragging) {
                    zox_log("   - bounded by [%s] by x[%i-%i] y[%i-%i]", zox_get_name(dragged_entity), drag_bounds.x, drag_bounds.y, drag_bounds.z, drag_bounds.w)
                }
            }
        }
    }
} zoxd_system2(ElementDragSystem);
