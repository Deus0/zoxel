const int max_mouse_delta = 120;
const float begin_rotate_power = 0.46; // 0.62
#define camera_quaternion_speed 0.004f // 0.01f

void FreeCameraRotateSystem(ecs_iter_t *it) {
    double rotate_power = begin_rotate_power * zox_delta_time * degreesToRadians * 32.0;
#ifdef zox_web
    rotate_power *= 10.0;
#endif
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

        zox_geter_value(camera->value, Roaming, byte, roaming);
        if (!roaming) {
            continue;
        }

        for (int j = 0; j < devices->length; j++) {
            const entity device = devices->value[j];
            if (!zox_valid(device) || zox_gett_value(device, DeviceDisabled)) {
                continue;
            }
            zox_geter(device, Children, zevices);
            for (int k = 0; k < zevices->length; k++) {
                const entity zevice = zevices->value[k];
                if (zox_has(device, Mouse)) {
                    if (!zox_has(zevice, ZevicePointerDelta)) {
                        continue;
                    }

                    const float2 delta = int2_to_float2(zox_gett_value(zevice, ZevicePointerDelta));
                    if (int_absf(delta.x) + int_absf(delta.y) >= max_mouse_delta || (delta.x == 0 &&delta.y == 0)) {
                        continue;
                    }

                    zox_muter(camera->value, Euler, euler);
                    float3 eulerAddition = { delta.y * rotate_power, -delta.x * rotate_power, 0 };
                    euler->value = float3_add(euler->value, eulerAddition);
                }
            }
        }
    }
} zoxd_system(FreeCameraRotateSystem)
