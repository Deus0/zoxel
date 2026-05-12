zox_sys2(Controller2MoveSystem) {
    init_delta_time()
    float2 max_delta_velocity = max_velocity2D;
    max_delta_velocity.x *= delta_time;
    max_delta_velocity.y *= delta_time;
    zox_sys_world()
    zox_sys_begin()
    zox_sys_in(DeviceLinks)
    zox_sys_in(CharacterLink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(DeviceLinks, deviceLinks)
        zox_sys_i(CharacterLink, characterLink)
        entity character = characterLink->value;
        if (!zox_valid(character) || !zox_has(character, Character2D)) {
            continue;
        }
        if (zox_has(character, DisableMovement)) {
            zox_geter(character, DisableMovement, disableMovement)
            if (disableMovement->value) {
                continue;
            }
        }
        byte is_running = 0;
        float2 movement = float2_zero; // { 0, 0 };
        float2 left_stick = float2_zero;
        // get the player input vector
        for (int j = 0; j < deviceLinks->length; j++) {
            entity e2 = deviceLinks->value[j];
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
                if (zox_has(e3, ZeviceStick)) {
                    zox_geter_value(e3, DeviceButtonType, byte, type);
                    if (type == zox_device_stick_left) {
                        zox_geter(e3, ZeviceStick, stick);
                        left_stick = stick->value;
                    }
                }
                if (zox_has(e3, ZeviceButton)) {
                    zox_geter_value(e3, DeviceButtonType, byte, type);
                    if (type == zox_device_button_lb || type == zox_device_button_rb) {
                        zox_geter_value(e3, ZeviceButton, byte, value);
                        if (!is_running && devices_get_pressed(value)) {
                            is_running = 1;
                        }
                    }
                }
            }
            if (zox_has(e2, Keyboard)) {
                zox_geter(e2, Keyboard, keyboard);
                if (keyboard->a.is_pressed) movement.x = -1;
                if (keyboard->d.is_pressed) movement.x = 1;
                if (keyboard->w.is_pressed) movement.y = 1;
                if (keyboard->s.is_pressed) movement.y = -1;
                if (keyboard->left_shift.is_pressed) is_running = 1;
            }
        }
        if (float_abs(left_stick.x) > joystick_cutoff_buffer) {
            movement.x = left_stick.x;
        }
        if (float_abs(left_stick.y) > joystick_cutoff_buffer) {
            movement.y = left_stick.y;
        }
        if (!movement.x && !movement.y) {
            continue;
        }
        movement.x *= player_movement_power2D.x;
        movement.y *= player_movement_power2D.y;
        if (is_running) { // gamepad->lb.is_pressed || gamepad->rb.is_pressed) {
            movement.x *= run_speed2D;
            movement.y *= run_speed2D;
        }
        zox_geter(character, Velocity2D, velocity2D)
        zox_muter(character, Acceleration2D, acceleration2D);
        // const float2 check_velocity = velocity2D->value;

        float2 max_speed = max_velocity2D;
        if (is_running) {
            max_speed.x *= run_speed2D;
            max_speed.y *= run_speed2D;
        }

        float2 potential_velocity_left = { velocity2D->value.x + (acceleration2D->value.x + movement.x) * delta_time, 0 };
        float2 potential_velocity_up = { 0, velocity2D->value.y + (acceleration2D->value.y + movement.y) * delta_time };

        if (float_abs(potential_velocity_left.x) < max_speed.x) {
            acceleration2D->value.x += movement.x;
        }
        if (float_abs(potential_velocity_up.y) < max_speed.y) {
            acceleration2D->value.y += movement.y;
        }
        // zox_log("movement: %fx%f", movement.x, movement.y)
    }
} zox_sys_end(Controller2MoveSystem);
