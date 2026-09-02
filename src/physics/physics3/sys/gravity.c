void gravity3_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_in(Gravity3D);
    zox_sys_out(Acceleration3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Gravity3D, gravity)
        zox_sys_o(Acceleration3D, acceleration);
        float3_add_float3_p(&acceleration->value, gravity->value);
    }
    zox_sys_on_end();
} zoxd_system(gravity3_system);
