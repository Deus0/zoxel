// NOTE: Swings a door's block vox about its hinge to its open state, door_swing_time to open or close
void door_swing_system(iter* it) {
    zox_sys_on_begin();
    const float step = zox_delta_time / door_swing_time;
    zox_sys_begin();
    zox_sys_in(DoorOpen);
    zox_sys_in(DoorRestPosition);
    zox_sys_in(DoorRestRotation);
    zox_sys_in(DoorHinge);
    zox_sys_in(Scale1);
    zox_sys_out(DoorSwing);
    zox_sys_out(Position3D);
    zox_sys_out(Rotation3D);
    zox_sys_out(TransformMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(DoorOpen, open);
        zox_sys_i(DoorRestPosition, rest);
        zox_sys_i(DoorRestRotation, rest_rotation);
        zox_sys_i(DoorHinge, hinge);
        zox_sys_i(Scale1, scale);
        zox_sys_o(DoorSwing, swing);
        zox_sys_o(Position3D, position);
        zox_sys_o(Rotation3D, rotation);
        zox_sys_o(TransformMatrix, matrix);
        const float target = open->value ? 1 : 0;
        if (swing->value == target) {
            continue;
        }
        swing->value = target > swing->value ? fminf(target, swing->value + step) : fmaxf(target, swing->value - step);
        door_swung(rest->value, rest_rotation->value, hinge->value, swing->value, &position->value, &rotation->value);
        matrix->value = float4x4_transform_scale(position->value, rotation->value, scale->value);
    }
    zox_sys_on_end();
} zoxd_system(door_swing_system);
