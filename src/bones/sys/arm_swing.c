void simple_swing_system(iter* it, byte limb) {
    float idle_speed = 0.1f;
    float idle_angle = 12;
    float walk_speed = 1.8f;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SwingState);
    zox_sys_in(SwingAngle);
    zox_sys_in(LocalPosition3D);
    zox_sys_out(LocalRotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SwingState, state);
        zox_sys_i(SwingAngle, swing_angle);
        zox_sys_i(LocalPosition3D, position);
        zox_sys_o(LocalRotation3D, rotation);
        if (!state->value) {
            continue;
        }
        byte moving = state->value == 2;
        float speed = moving ? walk_speed : idle_speed;
        float angle = moving ? swing_angle->value : idle_angle;
        byte is_right_side = position->value.x < 0;
        float swing = sin(M_PI * zox_current_time * speed);
        if (is_right_side) {
            swing = -swing;
        }
        // NOTE: Reverse direction for legs
        if (limb) {
            swing = -swing;
        }
        float3 euler = quaternion_to_euler(rotation->value);
        euler.x = (swing * angle * degrees_to_radians);
        euler.y = 0;
        euler.z = 0;
        rotation->value = euler_to_quaternion(euler);
    }
}

// NOTE: Simply raises arm up
zox_sys2(ArmSwingSystem) {
    simple_swing_system(it, 0);
} zox_sys_end(ArmSwingSystem);

zox_sys2(LegSwingSystem) {
    simple_swing_system(it, 1);
} zox_sys_end(LegSwingSystem);
