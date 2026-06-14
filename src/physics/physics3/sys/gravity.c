zox_sys2(Gravity3DSystem) {
    zox_sys_begin();
    zox_sys_in(Gravity3D);
    zox_sys_in(DisableGravity);
    zox_sys_out(Acceleration3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Gravity3D, gravity)
        zox_sys_i(DisableGravity, disabled);
        zox_sys_o(Acceleration3D, acceleration);
        if (!disabled->value) {
            float3_add_float3_p(&acceleration->value, gravity->value);
        }
    }
} zox_sys_end(Gravity3DSystem);
