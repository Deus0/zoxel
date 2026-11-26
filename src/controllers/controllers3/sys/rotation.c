void player3_rotate_system(iter* it) {
    zox_sys_on_begin();
    double gamepad_rotate_multiplier_x = 0.04;
    double gamepad_rotate_multiplier_y = 0.03;
    double mouse_rotate_multiplier = 0.0032; // 0.008;
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
        if (!zox_valid(character)) {
            continue;
        }
#ifdef zox_safety_checks
        if (!zox_has(character, Character3)) {
            zox_loge("Character [%s] Invalid Type");
            continue;
        }
        if (!zox_has(character, Euler)) {
            zox_loge("Character [%s] has no Euler");
            continue;
        }
        if (!zox_has(character, Rotation3D)) {
            zox_loge("Character [%s] has no Euler");
            continue;
        }
#endif
        if (zox_has(character, DisableMovement)) {
            continue;
        }
        entity camera = zox_get_link(world, e, CameraLink);
        byte camera_mode = zox_valid(camera) ?
            zox_getv(camera, CameraState) :
            zox_camera_state_first_person;
        // third person's orbit camera turns by the mouse itself (orbit.c), the character faces where it walks
        if (camera_mode != zox_camera_state_first_person) {
            continue;
        }
        float2 right_stick = float2_zero;
        float2 euler = float2_zero;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
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
                // For mouse pointer only, we rotate
                if (zox_has(e3, ZevicePointerDelta) &&
                    !zox_dbg_touch_with_mouse &&
                    zox_has(e3, MousePointer)) {
                    float2 delta = int2_to_float2(zox_getv(e3, ZevicePointerDelta));
                    euler.x = delta.y * mouse_rotate_multiplier;
                    euler.y = -delta.x * mouse_rotate_multiplier;
                }
                byte type = zox_getv(e3, DeviceButtonType);
                if (zox_has(e3, ZeviceStick)) {
                    if (type == zox_device_stick_right) {
                        float2 stick = zox_getv(e3, ZeviceStick);
                        right_stick.x += stick.x;
                        right_stick.y += stick.y;
                    }
                }
            }
        }
        // If not using mouse euler
        if (!euler.x && !euler.y) {
            if (float_abs(right_stick.x) < joystick_cutoff_buffer) {
                euler.y = 0;
            }
            if (float_abs(right_stick.y) < joystick_cutoff_buffer) {
                euler.x = 0;
            }
            // multiply by our stick modifiers
            euler.x = right_stick.y * gamepad_rotate_multiplier_y;
            euler.y = right_stick.x * gamepad_rotate_multiplier_x;
        }
        // a headset looks up and down itself (xr/fun/body.c turns the head), a stick's pitch would fight it
        if (camera_mode == zox_camera_state_third_person || zox_xr_active) {
            euler.x = 0;
        }
        if (!euler.x && !euler.y) {
            continue;
        }
        // todo: effect only rotation of axis for this
        // effect characters euler
        zox_muter(character, Euler, character_euler);
        zox_muter(character, Rotation3D, character_rotation);
        character_euler->value.y += euler.y;   // its heading, saved (characters3 saving)
        // turned about its own up (on a ceiling or a wall too: gravity_upright_system stands it on its gravity)
        character_rotation->value = float4_normalize(quaternion_rotate(character_rotation->value,
            quaternion_from_axis_angle(float3_up, euler.y)));
        // Set Rotation
        entity skeleton = get_skeleton(world, character);
        entity head_bone = zox_valid(skeleton) ?
            zox_get_link(world, skeleton, HeadBoneLink) :
            0;
        if (!zox_valid(head_bone)) {
            continue;
        }
        head_bone = zox_get_child_by_id(world, head_bone, zox_id(Camera));
        if (!zox_valid(head_bone)) {
            continue;
        }
        zox_muter(head_bone, LocalRotation3D, head_rotation);
        // NOTE: Positive for headbone, negative for camera
        head_rotation->value = camera_pitch_add(head_rotation->value, euler.x, 89 * degrees_to_radians);
    }
    zox_sys_on_end();
} zoxd_system(player3_rotate_system);
