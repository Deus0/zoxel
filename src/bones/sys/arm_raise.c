// NOTE: Simply raises arm up
zox_sys2(ShoulderRaiseSystem) {
    byte dbg_log = 0;
    float swing_angle_x = -28;
    float swing_angle_y = 14;
    float swing_angle_z = 8;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RaiseShoulder);
    zox_sys_in(ShoulderBoneLink);
    zox_sys_in(HeadBoneLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RaiseShoulder, state);
        zox_sys_i(ShoulderBoneLink, shoulder);
        zox_sys_i(HeadBoneLink, head);
        if (!zox_valid(shoulder->value)) {
            if (dbg_log) {
                zox_logw("Should is invalid on [%s]", zox_getn(e));
            }
            continue;
        }
        zox_muter(shoulder->value, LocalRotation3D, rotation);
        zox_muter(shoulder->value, SwingState, swing);
        if (!state->value) {
            rotation->value = quaternion_identity;  // set to original
            if (!swing->value) {
                swing->value = 1;
            }
            continue;
        }
        entity head_bone = head->value;
        if (!zox_valid(head_bone)) {
            continue;
        }
        entity camera = zox_get_child_by_id(world, head->value, zox_id(Camera));
        if (zox_valid(camera)) {
            head_bone = camera;
        }
        float4 head_rotation = zox_getv(head_bone, LocalRotation3D);
        float3 euler = quaternion_to_euler(head_rotation);
        float3 target = (float3) {
            -euler.x - 115 * degrees_to_radians,
            0, 0 };
        if (zox_has(e, SwingStart)) {
            float swing_speed = zox_getv(e, SwingSpeed);
            double time_passed = zox_current_time - zox_getv(e, SwingStart);
            if (time_passed < swing_speed) {
                float swing_bonus = (1 + sin(M_PI * (time_passed / swing_speed)));
                target.x += swing_angle_x * swing_bonus * degrees_to_radians;
                target.y += swing_angle_y * swing_bonus * degrees_to_radians;
                target.z += swing_angle_z * swing_bonus * degrees_to_radians;
                if (dbg_log) {
                    zox_log("Swinging: Time Passed [%f], Angle [%fx%f], Speed [%f]", time_passed, swing_angle_x * swing_bonus, swing_angle_y * swing_bonus, swing_speed);
                }
            }
        }
        euler = target;
        rotation->value = euler_to_quaternion(euler);
        // NOTE: Disable Swing
        swing->value = 0;
        // zox_log("Compare Euler: Real: [%f] Target: [%f]", euler.x * radians_to_degrees, test_euler.x * radians_to_degrees);
    }
} zox_sys_end(ShoulderRaiseSystem);
