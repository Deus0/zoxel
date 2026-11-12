// Rolling3D: grounded bodies roll over the ground without slipping (balls, barrels, dropped loot)
//  - radius is the half height, Bounds3D.y
//  - only while Grounded (ground_state_grounded), in the air it keeps the spin it had
//      (Frictioned bodies slow it with omega_dissipation3)
//  - runs before the spin systems, so it turns this frame
//  - Frictioned: the spin decays over the step, so it starts a little faster, to still turn
//      exactly distance / radius
static inline void physics3_roll(iter* it, const float c) {
    const float dt = (float) zox_delta_time;
    // the step turns w g (see physics_step), so turning w dt needs w scaled by dt / g
    const physics_step step = physics_step_new(c, dt);
    const float boost = step.g > 0 ? dt / step.g : 1;
    zox_physics_profile_begin();
    zox_sys_begin();
    zox_sys_in(Grounded);
    zox_sys_in(Velocity3D);
    zox_sys_in(Bounds3D);
    zox_sys_in(Rotation3D);
    zox_sys_out(Omega3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Grounded, grounded);
        zox_sys_i(Velocity3D, velocity);
        zox_sys_i(Bounds3D, bounds);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_o(Omega3D, omega);
        if (grounded->value != 1) { // ground_state_grounded
            continue;
        }
        const float3 world_omega = physics3_rolling_omega(velocity->value, bounds->value.y);
        omega->value = float3_scale(physics3_world_to_body(rotation->value, world_omega), boost);
    }
    zox_physics_profile_end();
}

void rolling3_system(iter* it) {
    zox_sys_on_begin();
    if (zox_delta_time > zox_physics_max_delta_time) {
        return;
    }
    physics3_roll(it, 0);
    zox_sys_on_end();
} zoxd_system(rolling3_system);

void rolling3_damped_system(iter* it) {
    zox_sys_on_begin();
    if (zox_delta_time > zox_physics_max_delta_time) {
        return;
    }
    physics3_roll(it, omega_dissipation3);
    zox_sys_on_end();
} zoxd_system(rolling3_damped_system);
