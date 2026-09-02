void acceleration3_system(iter* it) {
    if (zox_delta_time >= zox_physics_max_delta_time) {
        return;
    }
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_out(Acceleration3D);
    zox_sys_out(Velocity3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Acceleration3D, acceleration);
        zox_sys_o(Velocity3D, velocity);
        velocity->value = float3_add(
            velocity->value,
            float3_scale(
                acceleration->value,
                zox_delta_time));
        float3_make_zero(&acceleration->value);
    }
    zox_sys_on_end();
} zoxd_system(acceleration3_system);
