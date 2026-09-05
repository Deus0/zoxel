/*
 * ==========================================
 *          PLAYER MOVEMENT SYSTEM
 * ==========================================
 *
 * This is the magic behind the player's smooth and responsive movement!
 * Ever wondered how your character darts and dashes through the game world with such finesse?
 * Look no further! This system ensures your player obeys the laws of physics while still
 * feeling like a nimble acrobat.
 *
 * Features:
 * - Supports walking, running, and everything in between!
 * - Ensures speed limits are respected (no speedsters allowed beyond the max speed!).
 * - Seamlessly integrates with your game world's orientation, so movement always feels natural.
 *
 * Enjoy the thrill of movement, and remember: with great speed comes great responsibility!
 *
 * ==========================================
 * Code responsibly, have fun, and may your bugs be few and far between!
 * ==========================================
 */

#ifdef zox_debug_player_movement_direction
float debug_thickness = 2.0f;
extern entity spawn_line3(ecs*, float3, float3, float, double);
#endif

zox_sys2(Player3DMoveSystem) {
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        if (state->value != zox_player_state_playing) {
            continue;
        }
        entity character = zox_get_link(world, e, Character);
        if (!zox_valid(character) || !zox_has(character, Character3)) {
            continue;
        }
        if (zox_has(character, DisableMovement)) {
            continue;
        }
        entity camera = zox_get_link(world, character, Camera);
        byte camera_mode = zox_valid(camera) ?
            zox_getv(camera, CameraState) :
            zox_camera_state_first_person;
        if (camera_mode == zox_camera_state_free) {
            continue;
        }
        byte is_running = 0;
        float2 left_stick = float2_zero;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
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
                // NOTE: Disabled for Fingers now
                if (!zox_valid(e3) && !zox_has(e3, Finger)) {
                    continue;
                }
                if (zox_getv(e3, ZeviceDisabled)) {
                    continue;
                }
                byte type = zox_getv(e3, DeviceButtonType);
                if (zox_has(e3, ZeviceStick)) {
                    if (type == zox_device_stick_left) {
                        float2 stick = zox_getv(e3, ZeviceStick);
                        left_stick.y += stick.y;
                        // NOTE: We must invert our Left Stick X Axis
                        left_stick.x += -stick.x;
                    }
                }
                if (zox_has(e3, ZeviceButton)) {
                    if (type == zox_btn_x) {
                        zox_geter(e3, ZeviceButton, zeviceButton);
                        if (!is_running && devices_get_pressed(zeviceButton->value)) {
                            is_running = 1;
                        }
                    }
                }
            }
            if (zox_has(e2, Keyboard) && !zox_dbg_touch_with_mouse) {
                zox_geter(e2, Keyboard, keyboard);
                if (keyboard->s.is_pressed) left_stick.y += -1;
                if (keyboard->w.is_pressed) left_stick.y += 1;
                if (keyboard->a.is_pressed) left_stick.x += -1;
                if (keyboard->d.is_pressed) left_stick.x += 1;
                if (keyboard->left_shift.is_pressed) is_running = 1;
            }
        }
        if (left_stick.x == 0 && left_stick.y == 0) {
            continue;
        }
        // NOTE: Z axis is negative forward
        float3 movement = { left_stick.x * player_movement_power.x, 0, -left_stick.y * player_movement_power.y };
        if (is_camera_positive_z) {
            movement.x *= -1;
            movement.z *= -1;
        }
        if (is_running) {
            if (!zox_getv(character, FlyMode)) {
                movement.x *= run_accceleration;
                movement.y *= run_accceleration;
            } else {
                movement.x *= fly_run_acc_xz;
                movement.y *= fly_run_acc_xz;
            }
        }
        float4 movement_rotation = float4_identity;
        float4 character_rotation = zox_getv(character, Rotation3D);
        zox_geter(character, Velocity3D, velocity3D);
        zox_muter(character, Acceleration3D, acceleration);
        if (camera_mode == zox_camera_state_topdown ||
            camera_mode == zox_camera_state_ortho)
        {
            if (zox_valid(camera)) {
                float4 camera_rotation = zox_getv(camera, Rotation3D);
                float3 camera_euler = quaternion_to_euler(camera_rotation);
                camera_euler.x = 0;
                camera_euler.z = 0;
                movement_rotation = euler_to_quaternion(camera_euler);
                // Align Character to new move rotation!
                zox_muter(character, Rotation3D, character_rotation_mut);
                character_rotation_mut->value = movement_rotation;
#ifdef zox_debug_player_movement_direction
                const Position3D *position3D = zox_get(character, Position3D)
                spawn_line3(world, position3D->value, float3_add(position3D->value, movement), debug_thickness, 34.0);
                float3 movement2 = float4_rotate_float3(face_direction, (float3) { 0, 0, -1 });
                spawn_line3(world, position3D->value, float3_add(position3D->value, movement2), debug_thickness, 34.0);
                zox_log(" > face_direction %fx%fx%fx%f\n", face_direction.x, face_direction.y, face_direction.z, face_direction.w);
#endif
            }
        } else {
            movement_rotation = character_rotation;
        }
        // here we use two vectors for movement directions so we can limit them
        // we also use a potential velocity based on a calculated new velocity
        // (although we dont know delta_time next frame)
        float2 max_speed = max_velocity3D;
        if (is_running) {
            if (!zox_getv(character, FlyMode)) {
                max_speed.x *= run_speed;
                max_speed.y *= run_speed;
            } else {
                max_speed.x *= fly_run_speed;
                max_speed.y *= fly_run_speed;
            }
        }
        float3 movement_real_z = float4_rotate_float3(movement_rotation, (float3) { 0, 0, movement.z });
        movement_real_z.y = 0;
        float3 movement_real_x = float4_rotate_float3(movement_rotation, (float3) { movement.x, 0, 0 });
        movement_real_x.y = 0;
        // NOTE: We use inverse to get the XZ velocity, local movement velocity aligned to character
        float4 inverse_rotation = float4_inverse(character_rotation);
        float3 velocity_local = float4_rotate_float3(inverse_rotation, velocity3D->value);
        float3 acceleration_local = float4_rotate_float3(inverse_rotation, acceleration->value);
        float3 potential_velocity_forward = { 0, 0, velocity_local.z + (acceleration_local.z + movement.y) * delta_time };
        float3 potential_velocity_left = { velocity_local.x + (acceleration_local.x + movement.x) * delta_time, 0, 0 };
        if (float_abs(potential_velocity_forward.z) < max_speed.y) {
            acceleration->value = float3_add(acceleration->value, movement_real_z);
        }
        if (float_abs(potential_velocity_left.x) < max_speed.x) {
            acceleration->value = float3_add(acceleration->value, movement_real_x);
        }
#ifdef zox_debug_player_speed_limits
        if (float_abs(potential_velocity_left.x) < max_speed.x) zox_log(" > under maximum velocity x: %f\n", potential_velocity_left.x)
        else zox_log(" > past maximum velocity x: %f\n", potential_velocity_left.x)
        if (float_abs(potential_velocity_forward.z) < max_speed.y) zox_log(" > under maximum velocity z: %f\n", potential_velocity_forward.z)
        else zox_log(" > past maximum velocity z: %f\n", potential_velocity_forward.z)
#endif
    }
} zox_sys_end(Player3DMoveSystem);
