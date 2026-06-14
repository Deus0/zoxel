zox_sys2(Alpha3System) {
    if (zox_delta_time >= zox_physics_max_delta_time) return;
    zox_sys_begin();
    zox_sys_out(Alpha3D);
    zox_sys_out(Omega3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Alpha3D, alpha);
        zox_sys_o(Omega3D, omega);
        omega->value = float3_add(omega->value, float3_scale(alpha->value, zox_delta_time));
        float3_make_zero(&alpha->value);
    }
} zox_sys_end(Alpha3System);
