// spins bodies with an Alpha2D (angular acceleration): exact steps, then clears it
void Alpha2DSystem(iter* it) {
    zox_sys_on_begin();
    init_delta_time();
    const physics_step step = physics_step_new(0, (float) delta_time);
    zox_physics_profile_begin();
    zox_sys_begin();
    zox_sys_out(Alpha2D);
    zox_sys_out(Torque2D);
    zox_sys_out(Rotation2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Alpha2D, alpha2D);
        zox_sys_o(Torque2D, torque2D);
        zox_sys_o(Rotation2, rotation2D);
        const float turn = physics_step_apply(step, &torque2D->value, alpha2D->value);
        if (turn != 0) {
            rotation2D->value = physics2_wrap_angle(rotation2D->value + turn);
        }
        alpha2D->value = 0;
    }
    zox_physics_profile_end();
    zox_sys_on_end();
} zoxd_system(Alpha2DSystem);
