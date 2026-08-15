#ifdef zoxm_actions
// NOTE: Players keys to switch between actions
zox_sys2(ActionsShortcutSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        if (state->value != zox_player_state_playing) {
            continue;
        }
        byte is_shift_action_left = 0;
        byte is_shift_action_right = 0;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (!zox_valid(e2)) {
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
                if (zox_has(e3, ZeviceButton)) {
                    zox_geter_value(e3, ZeviceDisabled, byte, disabled);
                    if (disabled) {
                        continue;
                    }
                    byte device_button_type = zox_getv(e3, DeviceButtonType);
                    byte zevice_button = zox_getv(e3, ZeviceButton);
                    if (device_button_type == zox_btn_lb) {
                        if (devices_get_pressed_this_frame(zevice_button)) {
                            is_shift_action_left = 1;
                        }
                    } else if (device_button_type == zox_btn_rb) {
                        if (devices_get_pressed_this_frame(zevice_button)) {
                            is_shift_action_right = 1;
                        }
                    }
                    byte real_button_index = zox_getv(e3, RealButtonIndex);
                    if (real_button_index == zox_btn_dpad_left) {
                        if (devices_get_pressed_this_frame(zevice_button)) {
                            is_shift_action_left = 1;
                        }
                    } else if (real_button_index == zox_btn_dpad_right) {
                        if (devices_get_pressed_this_frame(zevice_button)) {
                            is_shift_action_right = 1;
                        }
                    }
                }
                if (zox_has(e3, ZeviceWheel)) {
                    int2 wheel = zox_getv(e3, ZeviceWheel);
                    if (wheel.y > 0) {
                        is_shift_action_right = 1;
                    } else if (wheel.y < 0) {
                        is_shift_action_left = 1;
                    }
                }
            }
            if (zox_has(e2, Keyboard)) {
                zox_geter(e2, Keyboard, keyboard);
                if (keyboard->_1.pressed_this_frame) {
                    set_player_action(world, e, 0);
                } else if (keyboard->_2.pressed_this_frame) set_player_action(world, e, 1);
                else if (keyboard->_3.pressed_this_frame) set_player_action(world, e, 2);
                else if (keyboard->_4.pressed_this_frame) set_player_action(world, e, 3);
                else if (keyboard->_5.pressed_this_frame) set_player_action(world, e, 4);
                else if (keyboard->_6.pressed_this_frame) set_player_action(world, e, 5);
                else if (keyboard->_7.pressed_this_frame) set_player_action(world, e, 6);
                else if (keyboard->_8.pressed_this_frame) set_player_action(world, e, 7);
            }
        }
        if (is_shift_action_left) {
            player_action_ui_move(world, e, -1);
        } else if (is_shift_action_right) {
            player_action_ui_move(world, e, 1);
        }
    }
} zox_sys_end(ActionsShortcutSystem);

#endif
