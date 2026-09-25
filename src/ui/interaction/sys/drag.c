zox_sys2(DraggerEndSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(DraggableState);
    zox_sys_out(DraggingDelta);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(DraggableState, state);
        zox_sys_o(DraggingDelta, delta);
        if (!state->value) {
            continue;
        }
        entity dragger = zox_get_link(world, e, DraggerLink);
        if (!dragger) {
            continue;
        }
        byte did_drag_end = 0;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(
            world,
            dragger,
            devices,
            zox_children_capacity,
            zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (!zox_valid(e2) ||
                zox_has(e2, Disabled))
            {
                continue;
            }
            iter it2 = zox_children(world, e2);
            while (zox_children_next(it2)) {
                for (int k = 0; k < it2.count; k++) {
                    entity e3 = it2.entities[k];
                    if (!zox_valid(e3) ||
                        !zox_has(e3, ZevicePointer))
                    {
                        continue;
                    }
                    byte click = zox_getv(e3, ZevicePointer);
                    int2 zdelta = zox_getv(e3, ZevicePointerDelta);
                    if (devices_get_released_this_frame(click)) {
                        did_drag_end = 1;
                    } else if (devices_get_pressed(click)) {
                        delta->value = zdelta;
                    }
                }
            }
        }
        if (did_drag_end) {
            state->value = 0;
            delta->value = int2_zero;
            zox_unlink(world, e, DraggerLink, dragger);
            if (is_log_dragging) {
                zox_log("Dragging Ended!");
            }
        }
    }
} zox_sys_end(DraggerEndSystem);
