// moves bodies with a velocity but no Acceleration2D (those move in acceleration2d_system)
void velocity2d_system(iter* it) { // Veloco Rapters!
    zox_sys_on_begin();
    init_delta_time();
    zox_physics_profile_begin();
    zox_sys_begin();
    zox_sys_in(Velocity2D);
    zox_sys_out(Position2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Velocity2D, velocity2D);
        zox_sys_o(Position2, position2);
        position2->value.x += velocity2D->value.x * delta_time;
        position2->value.y += velocity2D->value.y * delta_time;
    }
    zox_physics_profile_end();
    zox_sys_on_end();
} zoxd_system(velocity2d_system);
