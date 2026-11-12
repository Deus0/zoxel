// Torque2D is the 2D angular velocity (radians per second), named before Alpha2D existed
// spins bodies without an Alpha2D (those spin in Alpha2DSystem)
// the angle is kept within (-2pi, 2pi), spinning either way, however fast
void Torque2DSystem(iter* it) {
    zox_sys_on_begin();
    init_delta_time();
    zox_physics_profile_begin();
    zox_sys_begin();
    zox_sys_out(Rotation2);
    zox_sys_in(Torque2D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Rotation2, rotation2D);
        zox_sys_i(Torque2D, torque2D);
        if (torque2D->value == 0) {
            continue;
        }
        rotation2D->value = physics2_wrap_angle(rotation2D->value + torque2D->value * delta_time);
    }
    zox_physics_profile_end();
    zox_sys_on_end();
} zoxd_system(Torque2DSystem);
