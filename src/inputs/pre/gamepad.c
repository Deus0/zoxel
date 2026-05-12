entity spawn_prefab_gamepad(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("gamepad");
    zox_add_tag(e, Gamepad);
    zox_prefab_set(e, DeviceLayout, { 0 });
    return e;
}

entity spawn_gamepad(ecs *world, byte gamepad_type) {
    zox_instance(prefab_gamepad);
    zox_name("gamepad");
    zox_set(e, DeviceLayout, { gamepad_type });
    const byte *button_map = NULL;
    const byte *stick_map = NULL;
    const byte *dpad_map = NULL;
    switch (gamepad_type) {
        case zox_gamepad_layout_type_xbox:
            button_map = generic_button_map;
            stick_map = generic_stick_map;
            dpad_map = generic_dpad_map;
            break;
        case zox_gamepad_layout_type_steamdeck:
            button_map = steamdeck_button_map;
            stick_map = steamdeck_stick_map;
            dpad_map = generic_dpad_map;
            break;
        default:
            button_map = generic_button_map;
            stick_map = generic_stick_map;
            dpad_map = generic_dpad_map;
            break;
    }
    // byte total = zox_gamepad_button_count + zox_gamepad_stick_count + zox_gamepad_dpad_count;
    byte i = 0;
    // Buttons
    for (byte j = 0; j < zox_gamepad_button_count; j++, i++) {
        entity e2 = spawn_device_button(world, prefab_zevice_button, i, button_map[j]);
        zox_set_parent(world, e2, e);
    }
    // Sticks
    for (byte j = 0; j < zox_gamepad_stick_count; j++, i++) {
        entity e2 = spawn_zevice_stick(world, e, j, stick_map[j]);
        zox_set_parent(world, e2, e);
    }
    // The DPAD
    for (byte j = 0; j < zox_gamepad_dpad_count; j++, i++) {
        entity e2 = spawn_device_button(world, prefab_zevice_button, i, dpad_map[j]);
        zox_set_parent(world, e2, e);
    }
    // todo: spawn LT and RT as axis for steamdeck
    gamepad_entity = e;
    return e;
}


    // spawns zevice_stick at 14, 15
    /*for (byte i = 0; i < 2; i++) {
        byte j = 14 + i;
        byte joystick_index;
        if (gamepad_type == zox_gamepad_layout_type_steamdeck) {
            if (i == 0) joystick_index = 0; // 0, 1
            else joystick_index = 3; // 3, 4
        } else {
            joystick_index = i * 2;
        }
        children.value[j] = spawn_zevice_stick(world, e, i, joystick_index);
    }*/


        // use gamepad layout
        /*byte joystick_index = 0;
        if (gamepad_type == zox_gamepad_layout_type_xbox) {
            if (i == zox_device_button_lt) joystick_index = 11;
            else if (i == zox_device_button_rt) joystick_index = 12;
            else if (i == zox_device_button_select) joystick_index = 6;
            else if (i == zox_device_button_start) joystick_index = 7;
            else if (i == zox_device_button_left_stick_push) joystick_index = 9;
            else if (i == zox_device_button_right_stick_push) joystick_index = 10;
            else joystick_index = i;
        } else if (gamepad_type == zox_gamepad_layout_type_steamdeck) {
            // buttons
            if (i == zox_device_button_select) joystick_index = 6;
            else if (i == zox_device_button_start) joystick_index = 7;
            else if (i == zox_device_button_a) joystick_index = 0;
            else if (i == zox_device_button_b) joystick_index = 1;
            else if (i == zox_device_button_x) joystick_index = 2;
            else if (i == zox_device_button_y) joystick_index = 3;
            // bumpers
            else if (i == zox_device_button_lb) joystick_index = 4;
            else if (i == zox_device_button_rb) joystick_index = 5;
            else if (i == zox_device_button_lt) joystick_index = 8;
            else if (i == zox_device_button_rt) joystick_index = 9;
            // joysticks
            else if (i == zox_device_button_left_stick_push) joystick_index = 10;
            else if (i == zox_device_button_right_stick_push) joystick_index = 11;
            else joystick_index = i;
        } else {
            if (i == zox_device_button_x) joystick_index = 3;
            else if (i == zox_device_button_y) joystick_index = 4;
            else if (i == zox_device_button_lb) joystick_index = 6;
            else if (i == zox_device_button_rb) joystick_index = 7;
            else if (i == zox_device_button_lt) joystick_index = 8;
            else if (i == zox_device_button_rt) joystick_index = 9;
            else if (i == zox_device_button_select) joystick_index = 10;
            else if (i == zox_device_button_start) joystick_index = 11;
            else if (i == zox_device_button_left_stick_push) joystick_index = 13;
            else if (i == zox_device_button_right_stick_push) joystick_index = 14;
            else joystick_index = i;
        }
        children.value[i] = spawn_device_button(world, i, joystick_index);*/
