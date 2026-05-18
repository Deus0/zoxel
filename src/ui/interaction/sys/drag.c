zox_sys2(DraggerEndSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(DraggableState);
    zox_sys_out(DraggerLink);
    zox_sys_out(DraggingDelta);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(DraggerLink, dragger);
        zox_sys_o(DraggableState, state);
        zox_sys_o(DraggingDelta, delta);
        if (!dragger->value || !state->value) {
            continue;
        }
        zox_geter(dragger->value, DeviceLinks, devices);
        byte did_drag_end = 0;
        for (int j = 0; j < devices->length; j++) {
            entity e2 = devices->value[j];
            if (!zox_valid(e2) || zox_gett_value(e2, DeviceDisabled)) {
                continue;
            }
            uint children_capacity = zox_children_capacity;
            entity children[children_capacity];
            uint children_length = zox_get_children(world, e2, children, children_capacity);
            for (uint k = 0; k < children_length; k++) {
                entity e3 = children[k];
                if (!zox_valid(e3) || !zox_has(e3, ZevicePointer)) {
                    continue;
                }
                zox_geter_value(e3, ZevicePointer, byte, click);
                zox_geter_value(e3, ZevicePointerDelta, int2, zdelta);
                if (devices_get_released_this_frame(click)) {
                    did_drag_end = 1;
                } else if (devices_get_pressed(click)) {
                    delta->value = zdelta;
                }
            }
        }
        if (did_drag_end) {
            state->value = 0;
            dragger->value = 0;
            delta->value = int2_zero;

            if (is_log_dragging) {
                zox_log("Dragging Ended [%f]", (float) zox_current_time);
            }
        }
    }
} zox_sys_end(DraggerEndSystem);
