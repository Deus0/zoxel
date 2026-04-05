zox_sys2(Player3RotateSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceLinks);
    zox_sys_in(DeviceMode);
    zox_sys_in(CharacterLink);
    zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CharacterLink, characterLink)
        zox_sys_i(CameraLink, cameraLink)
        zox_sys_i(DeviceLinks, deviceLinks)
        zox_sys_i(DeviceMode, deviceMode)

        entity character = characterLink->value;
        if (!zox_valid(character) || !zox_has(character, Character3)) {
            continue;
        }

        zox_geter(character, DisableMovement, disableMovement)
        if (disableMovement->value) {
            continue;
        }

        byte camera_mode = zox_valid(cameraLink->value) ? zox_gett_value(cameraLink->value, CameraState) : zox_camera_state_first_person;
        if (camera_mode != zox_camera_state_first_person && camera_mode != zox_camera_state_third_person) {
            continue;
        }

        float2 right_stick = float2_zero;
        float2 euler = float2_zero;
        for (int j = 0; j < deviceLinks->length; j++) {
            entity device = deviceLinks->value[j];

            if (!zox_valid(device)) {
                continue;
            }

            if (deviceMode->value == zox_device_mode_keyboardmouse && zox_has(device, Mouse)) {
                zox_geter(device, Children, zevices)
                for (int k = 0; k < zevices->length; k++) {
                    entity zevice = zevices->value[k];

                    if (!zox_has(zevice, ZevicePointerDelta)) {
                        continue;
                    }

                    float2 delta = int2_to_float2(zox_gett_value(zevice, ZevicePointerDelta));
                    euler.x = - delta.y * mouse_rotate_multiplier;
                    euler.y = - delta.x * mouse_rotate_multiplier;
                }
            } else if (deviceMode->value == zox_device_mode_gamepad && zox_has(device, Gamepad)) {
                zox_geter(device, Children, zevices)
                for (int k = 0; k < zevices->length; k++) {
                    entity zevice = zevices->value[k];

                    zox_geter(zevice, DeviceButtonType, deviceButtonType)
                    if (zox_has(zevice, ZeviceStick)) {
                        if (deviceButtonType->value == zox_device_stick_right) {
                            zox_geter(zevice, ZeviceDisabled, zeviceDisabled)
                            if (!zeviceDisabled->value) {
                                zox_geter(zevice, ZeviceStick, zeviceStick)
                                right_stick.x += zeviceStick->value.x;
                                right_stick.y -= zeviceStick->value.y;
                            }
                            break;
                        }
                    }
                }
            } else if (deviceMode->value == zox_device_mode_touchscreen && zox_has(device, Touchscreen)) {
                zox_geter(device, Children, zevices)
                for (int k = 0; k < zevices->length; k++) {
                    entity zevice = zevices->value[k];

                    if (zox_has(zevice, Finger)) {
                        continue;
                    }

                    zox_geter(zevice, ZeviceDisabled, zeviceDisabled)
                    if (zeviceDisabled->value) {
                        continue;
                    }

                    if (zox_has(zevice, ZeviceStick)) {
                        byte joystick_type = zox_get_value(zevice, DeviceButtonType)
                        if (joystick_type == zox_device_stick_right) {
                            zox_geter(zevice, ZeviceStick, zeviceStick)
                            right_stick.x -= zeviceStick->value.x * touchscreen_rotate_multiplier;
                            right_stick.y -= zeviceStick->value.y * touchscreen_rotate_multiplier;
                        }
                    }
                }
            }
        }

        if (float_abs(right_stick.x) >= joystick_cutoff_buffer) {
            if (right_stick.x < -joystick_cutoff_buffer) {
                euler.y = right_stick.x * gamepad_rotate_multiplier_x;
            } else if (right_stick.x > joystick_cutoff_buffer) {
                euler.y = right_stick.x * gamepad_rotate_multiplier_x;
            }
        }

        if (float_abs(right_stick.y) >= joystick_cutoff_buffer) {
            if (right_stick.y < -joystick_cutoff_buffer) {
                euler.x = right_stick.y * gamepad_rotate_multiplier_y;
            } else if (right_stick.y > joystick_cutoff_buffer) {
                euler.x = right_stick.y * gamepad_rotate_multiplier_y;
            }
        }


        if (camera_mode == zox_camera_state_third_person) {
            euler.x = 0;
        }

        if (euler.x == 0 && euler.y == 0) {
            continue;
        }
        if (zox_players_reverse_rotate_x) {
            euler.y *= -1;
        }
        if (zox_players_reverse_rotate_y) {
            euler.x *= -1;
        }
        // todo: effect only rotation of axis for this

        // effect characters euler
        zox_muter(character, Euler, character_euler);
        zox_muter(character, Rotation3D, character_rotation);

        character_euler->value.y += euler.y;
        character_rotation->value = quaternion_from_euler(character_euler->value);

        zox_geter_value(character, CameraLink, entity, camera);

        if (!zox_valid(camera)) {
            zox_logw("camera  invalid for rotation");
            continue;
        }

        // add mouse/device input (Y INPUT)
        zox_muter(camera, Euler, ceuler);
        ceuler->value.x -= euler.x * radians_to_degrees;

        zox_log("New Camera Euler X (%f)", ceuler->value.x);

        // makes sure to keep euler between values -180 and 180
        /*if (ceuler->value.x >= 180) {
            ceuler->value.x -= 360;
        } else if (ceuler->value.x < -180) {
            ceuler->value.x += 360;
        }*/

        // limit camera for player head
        float2 camera_limit_x = (float2) { 89, 89 };
        if (ceuler->value.x < -camera_limit_x.y) {
            ceuler->value.x = -camera_limit_x.y;
        }
        else if (ceuler->value.x > camera_limit_x.x)  {
            ceuler->value.x = camera_limit_x.x;
        }

        // TODO: Use LocalEuler and the override for this
        zox_muter(camera, LocalRotation3D, crotation);
        crotation->value = quaternion_from_euler(float3_scale(ceuler->value, degrees_to_radians));
    }
} zox_sys_end(Player3RotateSystem);
