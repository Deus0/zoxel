zox_sys2(Player3RotateSystem) {
    double gamepad_rotate_multiplier_x = 0.04;
    double gamepad_rotate_multiplier_y = 0.03;
    // float touchscreen_rotate_multiplier = 0.6f;
    double mouse_rotate_multiplier = 0.0032; // 0.008;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceLinks);
    zox_sys_in(CharacterLink);
    zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CharacterLink, characterLink);
        zox_sys_i(CameraLink, cameraLink);
        zox_sys_i(DeviceLinks, devices);
        entity character = characterLink->value;
        if (!zox_valid(character) || !zox_has(character, Character3)) {
            continue;
        }
        byte mdisabled = zox_getv(character, DisableMovement);
        if (mdisabled) {
            continue;
        }
        byte camera_mode = zox_valid(cameraLink->value) ? zox_getv(cameraLink->value, CameraState) : zox_camera_state_first_person;
        if (camera_mode != zox_camera_state_first_person && camera_mode != zox_camera_state_third_person) {
            continue;
        }
        float2 right_stick = float2_zero;
        float2 euler = float2_zero;
        for (int j = 0; j < devices->length; j++) {
            entity e2 = devices->value[j];
            if (!zox_valid(e2) || !zox_has(e2, DeviceDisabled)) {
                continue;
            }
            byte ddisabled = zox_getv(e2, DeviceDisabled);
            if (ddisabled) {
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
                byte zdisabled = zox_getv(e3, ZeviceDisabled);
                if (zdisabled) {
                    continue;
                }
                if (zox_has(e3, ZevicePointerDelta) && !zox_dbg_touch_with_mouse) {
                    float2 delta = int2_to_float2(zox_getv(e3, ZevicePointerDelta));
                    euler.x = - delta.y * mouse_rotate_multiplier;
                    euler.y = - delta.x * mouse_rotate_multiplier;
                }
                byte type = zox_getv(e3, DeviceButtonType);
                if (zox_has(e3, ZeviceStick)) {
                    if (type == zox_device_stick_right) {
                        float2 stick = zox_getv(e3, ZeviceStick);
                        right_stick.x += stick.x;
                        right_stick.y -= stick.y;
                    }
                }
            }
        }
        // If not using mouse euler
        if (!euler.x && !euler.y) {
            if (float_abs(right_stick.x) < joystick_cutoff_buffer) {
                euler.y = 0;
            }
            if (float_abs(right_stick.y) < joystick_cutoff_buffer) {
                euler.x = 0;
            }
            // multiply by our stick modifiers
            euler.y = right_stick.x * gamepad_rotate_multiplier_x;
            euler.x = right_stick.y * gamepad_rotate_multiplier_y;
        }
        if (camera_mode == zox_camera_state_third_person) {
            euler.x = 0;
        }
        if (!euler.x && !euler.y) {
            continue;
        }
        // todo: effect only rotation of axis for this
        // effect characters euler
        zox_muter(character, Euler, character_euler);
        zox_muter(character, Rotation3D, character_rotation);
        character_euler->value.y += euler.y;
        character_rotation->value = quaternion_from_euler(character_euler->value);
        entity camera = zox_getv(character, CameraLink);
        if (!zox_valid(camera)) {
            zox_logw("Camera  invalid for rotation");
            continue;
        }
        // add mouse/device input (Y INPUT)
        zox_muter(camera, Euler, ceuler);
        ceuler->value.x -= euler.x * radians_to_degrees;
        // limit camera for player head
        float2 camera_limit_x = (float2) { 89, 89 };
        if (ceuler->value.x < -camera_limit_x.y) {
            ceuler->value.x = -camera_limit_x.y;
        }
        else if (ceuler->value.x > camera_limit_x.x)  {
            ceuler->value.x = camera_limit_x.x;
        }
        // TODO: Use LocalEuler and the override for this, confusing to debug atm due to inconsistency
        zox_muter(camera, LocalRotation3D, crotation);
        crotation->value = quaternion_from_euler(float3_scale(ceuler->value, degrees_to_radians));
    }
} zox_sys_end(Player3RotateSystem);


/*if (zox_has(e3, ZeviceStick)) {
    byte joystick_type = zox_get_value(e3, DeviceButtonType)
    if (joystick_type == zox_device_stick_right) {
        zox_geter(e3, ZeviceStick, zeviceStick)
        right_stick.x -= zeviceStick->value.x * touchscreen_rotate_multiplier;
        right_stick.y -= zeviceStick->value.y * touchscreen_rotate_multiplier;
    }
}*/
