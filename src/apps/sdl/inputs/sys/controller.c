// const float stick_cutoff = 0.06f;

byte get_controller_button(
    byte old_value,
    zox_sdl_gamepad* controller,
    zox_sdl_gamepad_button btn,
    byte dbg_log)
{
    byte value = sdl_get_gamepad_button(controller, btn);
    /*if (value && dbg_log) {
        zox_log("SDL GameController Button [%i]: %i", btn, value);
    }*/
    return process_input_button(old_value, value);
}

float get_gamepad_axis_raw(
    zox_sdl_gamepad* controller,
    zox_sdl_gamepad_axis axis,
    byte dbg_log)
{
    Sint16 raw_value = sdl_get_gamepad_axis(controller, axis);
    // raw_value = apply_joystick_deadzone(axis, raw_value);
    float value = raw_value / 32768.0f;
    if (float_abs(value) <= joystick_min_cutoff) {
        value = 0.0f;
    }
    if (value > 1) {
        value = 1;
    }
    if (value < -1) {
        value = -1;
    }
    if (dbg_log && value) {
        zox_log("SDL Axis [%i]:[%i]", axis, value);
    }
    return value;
}

byte get_gamepad_axis(
    zox_sdl_gamepad* controller,
    ZeviceStick *stick,
    zox_sdl_gamepad_axis axis_x,
    zox_sdl_gamepad_axis axis_y,
    byte dbg_log)
{
    float2 previous_value = stick->value;
    stick->value.x = -get_gamepad_axis_raw(
        controller,
        axis_x,
        dbg_log);
    stick->value.y = -get_gamepad_axis_raw(
        controller,
        axis_y,
        dbg_log);
    if (stick->value.x || stick->value.y) {
        //float_abs(stick->value.x) > stick_cutoff ||
        //float_abs(stick->value.y) > stick_cutoff) {
        if (dbg_log) {
            zox_log(" > stick [%fx%f]", stick->value.x, stick->value.y);
        }
        return !(stick->value.x == previous_value.x &&
                 stick->value.y == previous_value.y);
    }
    return 0;
}

float get_gamepad_trigger(
    zox_sdl_gamepad* controller,
    zox_sdl_gamepad_axis axis,
    byte dbg_log)
{
    Sint16 raw = sdl_get_gamepad_axis(controller, axis);
    float value = (raw + 32768.0f) / 65535.0f;
    if (float_abs(value) <= joystick_min_cutoff) {
        value = 0.0f;
    }
    if (dbg_log && value) {
        // zox_log("Trigger Axis [%i]:[%f]", axis, value);
    }
    return value;
}

void sdl_controller_extract_button(
    ecs* world,
    entity e,
    zox_sdl_gamepad* controller,
    byte* any_inputs,
    byte dbg_log)
{
    byte rindex = zox_getv(e, RealButtonIndex);
    if (zox_has(e, ZeviceBumper)) {
        zox_sdl_gamepad_axis trigger;
        if (rindex == zox_stk_left) {
            trigger = ZOX_GAMEPAD_AXIS_LEFT_TRIGGER;
        } else {
            trigger = ZOX_GAMEPAD_AXIS_RIGHT_TRIGGER;
        }
        float value = get_gamepad_trigger(
            controller,
            trigger,
            dbg_log);
        byte button_value = value >= bumper_button_cutoff;
        zox_muter(e, ZeviceButton, button);
        byte new_value = process_input_button(
            button->value,
            button_value);
        if (button->value != new_value) {
            button->value = new_value;
            if (dbg_log) {
                zox_log("SDL Bumper Button [%i]: %f  %i",
                    rindex,
                    value,
                    new_value);
            }
        }
    } else if (zox_has(e, ZeviceStick)) {
        zox_mut_begin(e, ZeviceStick, stick);
        zox_sdl_gamepad_axis x, y;
        if (rindex == zox_stk_left) {
            x = ZOX_GAMEPAD_AXIS_LEFTX;
            y = ZOX_GAMEPAD_AXIS_LEFTY;
        } else {
            x = ZOX_GAMEPAD_AXIS_RIGHTX;
            y = ZOX_GAMEPAD_AXIS_RIGHTY;
        }
        get_gamepad_axis(
            controller,
            stick,
            x,
            y,
            dbg_log);
    } else if (zox_has(e, ZeviceButton)) {
        ZeviceButton* button = zox_mut(e, ZeviceButton);
        byte value = get_controller_button(
            button->value,
            controller,
            (zox_sdl_gamepad_button) rindex,
            dbg_log);
        if (value != button->value) {
            button->value = value;
            if (value && dbg_log) {
                zox_log("SDL GameController Button [%i]: %i",
                    rindex,
                    value);
            }
        }
        *any_inputs |= value;
    }
}

byte gamepad_is_any_input(ecs* world, entity e) {
    if (!e || !zox_valid(e)) {
        zox_loge("Invalid Gamepad in AnyInput");
        return 0;
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (zox_has(e2, ZeviceButton)) {
                byte button = zox_getv(e2, ZeviceButton);
                if (devices_get_pressed_this_frame(button)) {
                    return 1;
                }
            }
#ifndef zox_disable_gamepad_stick_as_any_input
            else if (zox_has(e2, ZeviceStick)) {
                byte2 stick = zox_getv(e2, ZeviceStick);
                return zevice_stick_has_input(stick, joystick_min_cutoff);
            }
#endif
        }
    }
    return 0;
}

void gamepad_extract_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(SdlGameController);
    zox_sys_out(DeviceHasInput);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(SdlGameController, controller);
        zox_sys_o(DeviceHasInput, any_input);
        // NOTE: Handle disconnections
        if (!zox_sdl_gamepad_connected(controller->value)) {
            zox_sdl_gamepad_close(controller->value);
            controller->value = NULL;
            if (dbg_log) {
                zox_log("SdlGameController Disconnected [%s]", zox_get_name(e));
            }
            entity player = zox_get_parent(world, e);
            if (zox_valid(player)) {
                zox_setv(player, DeviceMode, zox_device_mode_none);
            } else {
                zox_loge("No player parent of gamepad..");
            }
            zox_delete(e);
            continue;
        }
        // Fetch
        any_input->value = 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                sdl_controller_extract_button(
                    world,
                    e2,
                    controller->value,
                    &any_input->value,
                    dbg_log);
            }
        }
        // any_input->value = gamepad_is_any_input(world, e);
        if (dbg_log >= 2 && any_input->value) {
            zox_log("Gamepad [%s] has Any Input",
                zox_sys_e_name);
        }
    }
    zox_sys_on_end();
} zoxd_system(gamepad_extract_system);

// TODO: This later
/*void gamepad_button_extract_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity parent = zox_get_parent(world, e);
        const SdlGameController* controller = zox_get(parent, SdlGameController);
        sdl_controller_extract_button(
            world,
            e,
            controller->value,
            dbg_log);
    }
    zox_sys_on_end();
} zoxd_system(gamepad_button_extract_system);*/
