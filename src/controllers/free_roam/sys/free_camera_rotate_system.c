const int max_mouse_delta = 120;
const float begin_rotate_power = 0.46; // 0.62
#define camera_quaternion_speed 0.004f // 0.01f

zox_sys2(FreeCameraRotateSystem) {
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
            entity e2 = devices->value[j];
            if (!zox_valid(e2) || zox_gett_value(e2, DeviceDisabled)) {
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
                if (zox_has(e3, ZevicePointerDelta)) {
                    float2 delta = int2_to_float2(zox_gett_value(e3, ZevicePointerDelta));
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
} zox_sys_end(FreeCameraRotateSystem);
