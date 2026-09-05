byte toggle_camera_mode(ecs *world, entity camera) {
    zox_geter_value(camera, CameraState, byte, mode);
    if (mode == zox_camera_state_first_person) {
        mode = zox_camera_state_third_person;
    } else if (mode == zox_camera_state_third_person) {
        mode = zox_camera_state_topdown;
    } else {
        mode = zox_camera_state_first_person;
    }
    set_camera_mode(world, camera, mode);
    return mode;
}

zox_sys2(PlayerToggleCameraSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(CharacterLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(CharacterLink, character);
        if (state->value != zox_player_state_playing) {
            continue;
        }
        entity camera = zox_get_link(world, e, Camera);
        if (!zox_valid(camera) ||
            !zox_valid(character->value))
        {
            continue;
        }
        byte is_toggle = 0;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (!zox_valid(e2)) {
                continue;
            }
            if (!zox_has(e2, DeviceDisabled) || zox_getv(e2, DeviceDisabled)) {
                continue;
            }
            uint children_capacity = zox_children_capacity;
            entity children[children_capacity];
            uint children_length = zox_get_children(
                world,
                e2,
                children,
                children_capacity);
            for (uint k = 0; k < children_length; k++) {
                entity e3 = children[k];
                if (!zox_valid(e3)) {
                    continue;
                }
                zox_geter_value(e3, ZeviceDisabled, byte, disabled);
                if (disabled) {
                    continue;
                }
                if (zox_has(e3, ZeviceButton)) {
                    zox_geter(e3, DeviceButtonType, type);
                    if (type->value == zox_btn_right_stick_push) {
                        zox_geter_value(e3, ZeviceButton, byte, button);
                        if (devices_get_pressed_this_frame(button)) {
                            is_toggle = 1;
                            break;
                        }
                    }
                }
            }
            if (zox_has(e2, Keyboard)) {
                zox_geter(e2, Keyboard, keyboard);
                if (keyboard->f2.pressed_this_frame) {
                    is_toggle = 1;
                    break;
                }
            }
        }
        if (is_toggle) {
            entity canvas = zox_get_link(world, e, Canvas);
            byte mode = toggle_camera_mode(world, camera);
            byte is_first_person = mode == zox_camera_state_first_person;
            entity crosshair = zox_get_child_by_id(
                world,
                canvas,
                zox_id(Crosshair));
            if (zox_valid(crosshair)) {
                zox_setv(crosshair, RenderDisabled, !is_first_person);
            }
        }
    }
} zox_sys_end(PlayerToggleCameraSystem);
