// moves bodies with an Acceleration2D: exact steps (see physics/fun), then clears the forces
//  - Frictioned bodies also slow with drag friction2D, in the same exact step
static inline void physics2_accelerate(iter* it, const float k) {
    init_delta_time();
    const physics_step step = physics_step_new(k, (float) delta_time);
    zox_physics_profile_begin();
    zox_sys_begin();
    zox_sys_out(Velocity2D);
    zox_sys_out(Acceleration2D);
    zox_sys_out(Position2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Velocity2D, velocity2D);
        zox_sys_o(Acceleration2D, acceleration2D);
        zox_sys_o(Position2, position2);
        const float2 moved = physics_step_apply2(step, &velocity2D->value, acceleration2D->value);
        position2->value.x += moved.x;
        position2->value.y += moved.y;
        float2_make_zero(&acceleration2D->value);
    }
    zox_physics_profile_end();
}

void acceleration2d_system(iter* it) {
    zox_sys_on_begin();
    physics2_accelerate(it, 0);
    zox_sys_on_end();
} zoxd_system(acceleration2d_system);

void friction2d_system(iter* it) {
    zox_sys_on_begin();
    physics2_accelerate(it, friction2D);
    zox_sys_on_end();
} zoxd_system(friction2d_system);
