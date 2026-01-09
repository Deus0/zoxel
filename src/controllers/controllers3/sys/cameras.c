void on_set_free_roam(ecs *world, const entity e, byte is_free) {
    zox_geter_value(e, CameraLink, entity, camera);
    if (!is_free) {
        zox_geter_value(e, CharacterLink, entity, character);
        attach_camera_to_character(world, camera, character);
    } else {
        set_camera_free(world, camera);
    }
    /*if (zox_valid(local_menu_game)) {
        set_children_component_byte(
            world,
            local_menu_game,
            zox_id(RenderDisabled),
            is_free);
    } else {
        zox_logw("[local_menu_game] not found.");
    }*/
}

zox_sys2(PlayerToggleCameraSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(DeviceLinks);
    // zox_sys_in(GameLink);
    zox_sys_in(CharacterLink);
    zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(DeviceLinks, devices);
        // zox_sys_i(GameLink, gameLink);
        zox_sys_i(CharacterLink, characterLink);
        zox_sys_i(CameraLink, camera);

        if (state->value != zox_player_state_playing) {
            continue;
        }
        /*if (!zox_valid(gameLink->value) || !zox_valid(camera->value)) {
            continue;
        }

        zox_geter_value(gameLink->value, GameState, byte, game_state);
        if (game_state != zox_game_playing) {
            continue;
        }*/

        byte is_toggle_camera = 0;
        byte is_toggle_freeroam = 0;
        for (int j = 0; j < devices->length; j++) {

            const entity device = devices->value[j];
            if (!zox_valid(device)) {
                continue;
            }

            if (zox_has(device, Keyboard)) {
                const Keyboard *keyboard = zox_get(device, Keyboard)
                if (keyboard->f1.pressed_this_frame) {
                    is_toggle_camera = 1;
                } else if (keyboard->f2.pressed_this_frame) {
                    is_toggle_freeroam = 1;
                }
            } else if (zox_has(device, Gamepad)) {
                zox_geter(device, Children, zevices)
                for (int k = 0; k < zevices->length; k++) {
                    entity zevice_entity = zevices->value[k];
                    if (zox_has(zevice_entity, ZeviceButton)) {
                        zox_geter(zevice_entity, ZeviceDisabled, zeviceDisabled)
                        if (zeviceDisabled->value) {
                            continue;
                        }
                        zox_geter(zevice_entity, DeviceButtonType, deviceButtonType)
                        if (deviceButtonType->value == zox_device_button_right_stick_push) {
                            zox_geter(zevice_entity, ZeviceButton, zeviceButton)
                            if (devices_get_pressed_this_frame(zeviceButton->value)) {
                                is_toggle_camera = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }

        zox_geter_value(camera->value, CameraState, byte, camera_state);
        if (is_toggle_camera && zox_valid(characterLink->value)) {
            byte mode = toggle_camera_mode(world, camera->value);
            byte is_first_person = mode == zox_camera_state_first_person;
            zox_set(local_crosshair, RenderDisabled, { !is_first_person });
        } else if (is_toggle_freeroam) {

            byte is_free = camera_state != zox_camera_state_free;

            zox_log("Toggling free roam [%i] [%i]", state->value, is_free);
            zox_set(camera->value, CameraState, { is_free ? zox_camera_state_free : zox_camera_state_first_person });
            on_set_free_roam(world, e, is_free);
        }
    }
} zox_sys_end(PlayerToggleCameraSystem);