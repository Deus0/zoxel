// adds a constant force to Acceleration2D, opt in with a Gravity2D component
void gravity2d_system(iter* it) {
    zox_sys_on_begin();
    zox_physics_profile_begin();
    zox_sys_begin();
    zox_sys_in(Gravity2D);
    zox_sys_out(Acceleration2D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Gravity2D, gravity2D);
        zox_sys_o(Acceleration2D, acceleration2D);
        acceleration2D->value.x += gravity2D->value.x;
        acceleration2D->value.y += gravity2D->value.y;
    }
    zox_physics_profile_end();
    zox_sys_on_end();
} zoxd_system(gravity2d_system);
