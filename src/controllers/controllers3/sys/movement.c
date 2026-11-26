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

// a character's up, against its gravity (any direction: a gravity cube's flip), +y without one
static inline float3 character_up(ecs* world, entity character) {
    return physics3_up(zox_has(character, Gravity3D) ? zox_getv(character, Gravity3D) : float3_zero);
}

// the ground velocity straight from the stick (XR): along the ground at the walk's top speed, along the character's
//  facing, its part along up (falling, jumping) kept; scaled against this frame's drag, so it moves the same each frame (friction
//  kept, physics3_drag_compensation)
static inline void player3d_move_direct(ecs* world, entity character, float2 stick, byte is_running) {
    float3 local = { stick.x * max_velocity3D.x, 0, -stick.y * max_velocity3D.y };
    if (is_camera_positive_z) {
        local.x *= -1;
        local.z *= -1;
    }
    if (is_running) {
        local.x *= run_speed;
        local.z *= run_speed;
    }
    if (zox_has(character, InWater)) {
        local = float3_scale(local, swim_speed);
    }
    const float3 world_velocity = float3_scale(float4_rotate_float3(zox_getv(character, Rotation3D), local),
        physics3_drag_compensation(world, character));
    const float3 up = character_up(world, character);
    Velocity3D* velocity = zox_getm(character, Velocity3D);
    velocity->value = float3_add(float3_scale(up, float3_dot(velocity->value, up)), physics3_flatten(world_velocity, up));
}

void player3d_move_system(iter* it) {
    zox_sys_on_begin();
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
        entity character = zox_get_link(world, e, CharacterLink);
        if (!zox_valid(character) || !zox_has(character, Character3)) {
            continue;
        }
        if (zox_has(character, DisableMovement)) {
            continue;
        }
        entity camera = zox_get_link(world, character, CameraLink);
        byte camera_mode = zox_valid(camera) ?
            zox_getv(camera, CameraState) :
            zox_camera_state_first_person;
        if (camera_mode == zox_camera_state_free) {
            continue;
        }
        byte is_running = 0;
        byte crouch_held = 0;       // the left control key held
        byte crouch_toggled = 0;    // a gamepad's left stick pushed in this frame
        float2 left_stick = float2_zero;
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
            uint children_length = zox_get_children(
                world,
                e2,
                children,
                children_capacity);
            for (uint k = 0; k < children_length; k++) {
                entity e3 = children[k];
                if (zox_has(e3, Disabled)) {
                    continue;
                }
                // NOTE: Disabled for Fingers now
                if (zox_has(e3, Finger)) {
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
                        const ZeviceButton* zeviceButton = zox_get(e3, ZeviceButton);
                        if (!is_running && devices_get_pressed(zeviceButton->value)) {
                            is_running = 1;
                        }
                    } else if (type == zox_btn_left_stick_push && devices_get_pressed_this_frame(zox_getv(e3, ZeviceButton))) {
                        crouch_toggled = 1;
                    }
                }
            }
            if (zox_has(e2, Keyboard) && !zox_dbg_touch_with_mouse) {
                const Keyboard* keyboard = zox_get(e2, Keyboard);
                if (keyboard->s.is_pressed) left_stick.y += -1;
                if (keyboard->w.is_pressed) left_stick.y += 1;
                if (keyboard->a.is_pressed) left_stick.x += -1;
                if (keyboard->d.is_pressed) left_stick.x += 1;
                if (keyboard->left_shift.is_pressed) is_running = 1;
                if (keyboard->left_control.is_pressed) crouch_held = 1;
            }
        }
        // crouching: held by the left control key, toggled by a gamepad's left stick pushed in (holding a stick in
        //  while walking is awkward); the skeleton bends to it (bones/sys/arm_control.c), slower, no running. A
        //  headset crouches by the player's own body (xr/fun/body.c)
        byte crouching = 0;
        const entity skeleton = get_skeleton(world, character);
        if (!zox_xr_active && zox_valid(skeleton) && zox_has(skeleton, CrouchState)) {
            CrouchState* crouch = zox_getm(skeleton, CrouchState);
            byte value = crouch_held ? (crouch->value | 1) : (crouch->value & ~1);
            if (crouch_toggled) {
                value ^= 2;
            }
            crouch->value = value;
            crouching = value != 0;
        }
        if (crouching) {
            is_running = 0;
        }
        // a headset walks without inertia: the stick sets the ground velocity, at speed the frame it's pushed,
        //  stopped the frame it's let go (accelerating up and friction coasting down felt like lag, and sickens)
        if (zox_xr_active && !zox_getv(character, FlyMode)) {
            player3d_move_direct(world, character, left_stick, is_running);
            continue;
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
        const Velocity3D* velocity3D = zox_get(character, Velocity3D);
        zox_muter(character, Acceleration3D, acceleration);
        // moves along the camera's facing, the character turned to it (the orbit camera is third person's)
        if (camera_mode == zox_camera_state_topdown ||
            camera_mode == zox_camera_state_ortho ||
            camera_mode == zox_camera_state_third_person)
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
        if (crouching) {
            max_speed = float2_multiply1(max_speed, crouch_speed);
            movement.x *= crouch_speed;
            movement.z *= crouch_speed;
        }
        if (is_running) {
            if (!zox_getv(character, FlyMode)) {
                max_speed.x *= run_speed;
                max_speed.y *= run_speed;
            } else {
                max_speed.x *= fly_run_speed;
                max_speed.y *= fly_run_speed;
            }
        }
        if (zox_has(character, InWater)) {
            max_speed = float2_multiply1(max_speed, swim_speed);
        }
        // along the ground: flat against its up (its gravity's, any way)
        const float3 up = character_up(world, character);
        float3 movement_real_z = physics3_flatten(float4_rotate_float3(movement_rotation, (float3) { 0, 0, movement.z }), up);
        float3 movement_real_x = physics3_flatten(float4_rotate_float3(movement_rotation, (float3) { movement.x, 0, 0 }), up);
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
    zox_sys_on_end();
} zoxd_system(player3d_move_system);
