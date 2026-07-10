zox_sys2(FreeCameraToggleSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(CameraLink, camera);
        if (state->value != zox_player_state_playing || !zox_valid(camera->value)) {
            continue;
        }
        zox_geter_value(camera->value, CameraState, byte, camera_state);
        if (camera_state != zox_camera_state_free) {
            continue;
        }
        byte is_triggered = 0;
        entity mouse = 0;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (!zox_valid(e2) || zox_getv(e2, DeviceDisabled)) {
                continue;
            }
            uint children_capacity = zox_children_capacity;
            entity children[children_capacity];
            uint children_length = zox_get_children(world, e2, children, children_capacity);
            for (uint k = 0; k < children_length; k++) {
                entity e3 = children[k];
                if (!zox_valid(e3)) {
                    continue;
                }
                zox_geter_value(e3, ZeviceDisabled, byte, disabled);
                if (disabled) {
                    continue;
                }
                if (zox_has(e3, ZevicePointerRight)) {
                    zox_geter_value(e3, ZevicePointerRight, byte, click);
                    if (devices_get_pressed_this_frame(click)) {
                        is_triggered = 1;
                        mouse = e2;
                    }
                }
            }
        }
        if (is_triggered && mouse) {
            zox_geter_value(camera->value, Roaming, byte, roaming);
            byte new_roaming = !roaming;
            zox_set(mouse, MouseLock, { new_roaming });
            zox_set(camera->value, Roaming, { new_roaming });
        }
    }
} zox_sys_end(FreeCameraToggleSystem);
