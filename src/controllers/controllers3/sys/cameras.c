void set_camera_free(ecs *world, entity e) {
    // zox_set(e, CharacterLink, { 0 });

    zox_geter_value(e, Rotation3D, float4, camera_rotation3D);

    float3 euler = quaternion_to_euler(camera_rotation3D);

    zox_add_tag(e, EulerOverride);
    zox_set(e, Euler, { euler });
    zox_set(e, ParentLink, { 0 });
    if (camera_follow_mode == zox_camera_follow_mode_follow_xz) {
        zox_set(e, CameraFollowLink, { 0 })
    }
    if (local_mouse) zox_set(local_mouse, MouseLock, { 0 });
}

void attach_camera_to_character(ecs *world, entity e, entity character) {
    if (!zox_valid(character)) {
        zox_log_error("Invalid Character [attach_camera_to_character]");
        return;
    }

    // Initial Linking
    zox_set(e, CharacterLink, { character });
    zox_set(character, CameraLink, { e });

    // reset using head bone
    zox_set(e, CameraState, { zox_camera_state_first_person });
    zox_set(e, Roaming, { 0 });
    zox_set(e, ParentLink, { character });
    zox_remove_tag(e, EulerOverride);
    float3 euler = (float3) { 0, 180, 0 };
    zox_set(e, Euler, { euler });
    zox_set(e, LocalRotation3D, { quaternion_from_euler(float3_scale(euler, degreesToRadians)) });
    // zox_set(e, LocalRotation3D, { quaternion_identity });

    // set_camera_locked(world, e, character);
    // TODO: Add CameraDirty to Character for headbone adjustment system
    zox_set(character, SkeletonDirty, { zox_dirty_trigger });
    if (local_mouse) zox_set(local_mouse, MouseLock, { 1 });
}


zox_sys2(PlayerToggleCameraSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(DeviceLinks);
    zox_sys_in(CharacterLink);
    zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(DeviceLinks, devices);
        zox_sys_i(CharacterLink, character);
        zox_sys_i(CameraLink, camera);

        if (state->value != zox_player_state_playing) {
            continue;
        }

        if (!zox_valid(camera->value)) {
            continue;
        }

        byte is_toggle_camera = 0;
        byte is_toggle_freeroam = 0;
        for (int j = 0; j < devices->length; j++) {

            entity device = devices->value[j];

            if (!zox_valid(device)) {
                continue;
            }

            if (zox_has(device, Keyboard)) {
                zox_geter(device, Keyboard, keyboard);

                if (keyboard->f1.pressed_this_frame) {
                    is_toggle_freeroam = 1;
                }

                else if (keyboard->f2.pressed_this_frame) {
                    is_toggle_camera = 1;
                }

            } else if (zox_has(device, Gamepad)) {

                zox_geter(device, Children, zevices);
                for (int k = 0; k < zevices->length; k++) {
                    entity zevice = zevices->value[k];

                    if (!zox_has(zevice, ZeviceButton)) {
                        continue;
                    }

                    zox_geter(zevice, ZeviceDisabled, zeviceDisabled);
                    if (zeviceDisabled->value) {
                        continue;
                    }

                    zox_geter(zevice, DeviceButtonType, deviceButtonType);
                    if (deviceButtonType->value == zox_device_button_right_stick_push) {

                        zox_geter(zevice, ZeviceButton, zeviceButton);
                        if (devices_get_pressed_this_frame(zeviceButton->value)) {
                            is_toggle_camera = 1;
                            break;
                        }
                    }
                }
            }
        }

        if (is_toggle_camera && zox_valid(character->value)) {

            byte mode = toggle_camera_mode(world, camera->value);
            byte is_first_person = mode == zox_camera_state_first_person;
            zox_set(local_crosshair, RenderDisabled, { !is_first_person });

        } else if (is_toggle_freeroam) {

            zox_geter_value(camera->value, CameraState, byte, old);
            byte new = old;

            if (old == zox_camera_state_free) {
                new = zox_camera_state_first_person;
                attach_camera_to_character(world, camera->value, character->value);
            } else {
                new = zox_camera_state_free;
                set_camera_free(world, camera->value);
            }

            zox_set(camera->value, CameraState, { new });
            zox_log("= toggling free roam - old: [%i] -> new: [%i]", old, state->value);
        }
    }
} zox_sys_end(PlayerToggleCameraSystem);
