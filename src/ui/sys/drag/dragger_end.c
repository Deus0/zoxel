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
            entity device = devices->value[j];

            if (!zox_valid(device) || zox_gett_value(device, DeviceDisabled)) {
                continue;
            }

            zox_geter(device, Children, zevices);
            for (int k = 0; k < zevices->length; k++) {
                entity zevice = zevices->value[k];

                if (!zox_valid(zevice) || !zox_has(zevice, ZevicePointer)) {
                    continue;
                }

                zox_geter_value(zevice, ZevicePointer, byte, click);
                zox_geter_value(zevice, ZevicePointerDelta, int2, zdelta);

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



            /*if (state->value == zox_drag_mode_mouse && zox_has(device, Mouse)) {

            } else if (state->value == zox_drag_mode_finger && zox_has(device, Touchscreen)) {
                const Children *zevices = zox_get(device, Children)
                for (int k = 0; k < zevices->length; k++) {
                    entity zevice_entity = zevices->value[k];
                    if (zox_has(zevice_entity, ZevicePointer)) {
                        const ZevicePointer *zevicePointer = zox_get(zevice_entity, ZevicePointer)
                        if (devices_get_released_this_frame(zevicePointer->value)) did_drag_end = 1;
                        else if (devices_get_pressed(zevicePointer->value)) delta->value = zox_get_value(zevice_entity, ZevicePointerDelta);
                        break;
                    }
                }
            }*/
