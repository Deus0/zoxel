zox_sys2(FreeCameraMoveSystem) {
    double movement_power = zox_delta_time;
    movement_power *= movement_multiplier;
#ifdef zox_web
    movement_power *= 10.0f;
#endif
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity camera = zox_get_link(world, e, CameraLink);
        if (!zox_valid(camera) ||
            !zox_has(camera, Roaming)
        ) {
            continue;
        }
        /*zox_geter_value(camera->value, CameraState, byte, camera_state);
        if (camera_state != zox_camera_state_free) {
            continue;
        }*/
        float3 movement = { 0, 0, 0 };
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (!zox_valid(e2) || zox_getv(e2, DeviceDisabled)) {
                continue;
            }
            if (zox_has(e2, Keyboard)) {
                const Keyboard *keyboard = zox_get(e2, Keyboard);
                if (keyboard->a.is_pressed) movement.x += -1;
                if (keyboard->d.is_pressed) movement.x += 1;
                if (keyboard->w.is_pressed) movement.z = -1;
                if (keyboard->s.is_pressed) movement.z += 1;
                if (keyboard->q.is_pressed) movement.y += -1;
                if (keyboard->e.is_pressed) movement.y = 1;
                if (keyboard->left_shift.is_pressed) {
                    movement.x *= free_camera_run_multiplier;
                    movement.y *= free_camera_run_multiplier;
                    movement.z *= free_camera_run_multiplier;
                }
            }
        }
        if (movement.x == 0 && movement.y == 0 && movement.z == 0) {
            continue;
        }
        movement = float3_scale(movement, movement_power);
        float4 rotation = zox_getv(camera, Rotation3D);
        zox_muter(camera, Position3D, position);
        position->value =
            float3_add(position->value,
                float4_rotate_float3(rotation, movement));
    }
} zox_sys_end(FreeCameraMoveSystem);
