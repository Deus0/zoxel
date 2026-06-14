zox_sys2(Dissipation3System) {
    if (zox_delta_time >= zox_physics_max_delta_time) return;
    float dissipation_rate = 2.0f; // tweak for stronger or weaker drag
    zox_sys_begin();
    zox_sys_out(Omega3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Omega3D, omega);
        //float friction_factor = 1.0f - (dissipate_power * delta_time);
        //friction_factor = clampf(friction_factor, 0.0f, 1.0f); // Safety clamp
        //omega->value = float3_scale(omega->value, friction_factor);
        // Compute exponential decay factor
        float drag = expf(-dissipation_rate * zox_delta_time);
        drag = clampf(drag, 0.0f, 1.0f); // Safety clamp
        omega->value = float3_scale(omega->value, drag);
    }
} zox_sys_end(Dissipation3System);
