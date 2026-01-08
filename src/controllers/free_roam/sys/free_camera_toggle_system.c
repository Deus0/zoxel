void FreeCameraToggleSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(DeviceLinks);
    zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(PlayerState, state);
        zox_sys_i(CameraLink, camera);
        zox_sys_i(DeviceLinks, devices);

        if (state->value != zox_player_state_playing || !zox_valid(camera->value)) {
            continue;
        }

        zox_geter_value(camera->value, CameraState, byte, camera_state);
        if (camera_state != zox_camera_state_free) {
            continue;
        }

        byte is_triggered = 0;
        entity mouse = 0;
        for (int j = 0; j < devices->length; j++) {
            entity device = devices->value[j];

            if (!zox_valid(device) || zox_gett_value(device, DeviceDisabled)) {
                continue;
            }

            if (zox_has(device, Mouse)) {
                zox_geter(device, Children, zevices);
                for (int k = 0; k < zevices->length; k++) {
                    entity zevice = zevices->value[k];

                    if (!zox_has(zevice, ZevicePointerRight)) {
                        continue;
                    }

                    zox_geter_value(zevice, ZevicePointerRight, byte, click);

                    if (devices_get_pressed_this_frame(click)) {
                        is_triggered = 1;
                        mouse = device;
                    }
                }
            }
        }

        if (is_triggered && mouse) {
            zox_geter_value(camera->value, Roaming, byte, roaming);
            // zox_geter_value(camera->value, MouseLock, byte, mouse_locked);
            byte new_roaming = !roaming;
            zox_set(mouse, MouseLock, { new_roaming });
            zox_set(camera->value, Roaming, { new_roaming });
        }
    }
} zoxd_system2(FreeCameraToggleSystem);
