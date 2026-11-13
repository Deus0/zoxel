void controller2_move_system(iter* it) {
    zox_sys_on_begin();
    init_delta_time()
    float2 max_delta_velocity = max_velocity2D;
    max_delta_velocity.x *= delta_time;
    max_delta_velocity.y *= delta_time;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity character = zox_get_link(world, e, CharacterLink);
        if (!zox_valid(character) || !zox_has(character, Character2D)) {
            continue;
        }
        if (zox_has(character, DisableMovement)) {
            continue;
        }
        byte is_running = 0;
        float2 movement = float2_zero;
        float2 left_stick = float2_zero;
        // get the player input vector
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(
            world,
            e,
            devices,
            zox_children_capacity,
            zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (zox_has(e2, Disabled)) {
                continue;
            }
            uint children_capacity = zox_children_capacity;
            entity children[children_capacity];
            uint children_length = zox_get_children(world, e2, children, children_capacity);
            for (uint k = 0; k < children_length; k++) {
                entity e3 = children[k];
                if (zox_has(e3, Disabled)) {
                    continue;
                }
                if (zox_has(e3, ZeviceStick)) {
                    byte type = zox_getv(e3, DeviceButtonType);
                    if (type == zox_device_stick_left) {
                        const ZeviceStick* stick = zox_get(e3, ZeviceStick);
                        left_stick = stick->value;
                    }
                }
                if (zox_has(e3, ZeviceButton)) {
                    byte type = zox_getv(e3, DeviceButtonType);
                    if (type == zox_btn_lb || type == zox_btn_rb) {
                        byte value = zox_getv(e3, ZeviceButton);
                        if (!is_running && devices_get_pressed(value)) {
                            is_running = 1;
                        }
                    }
                }
            }
            if (zox_has(e2, Keyboard)) {
                const Keyboard* keyboard = zox_get(e2, Keyboard);
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
        const Velocity2D* velocity2D = zox_get(character, Velocity2D);
        zox_muter(character, Acceleration2D, acceleration2D);
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
    zox_sys_on_end();
} zoxd_system(controller2_move_system);
