void euler_override_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_in(Euler);
    zox_sys_out(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Euler, euler);
        zox_sys_o(Rotation3D, rotation);

        rotation->value = quaternion_from_euler(euler->value);
    }
    zox_sys_on_end();
} zoxd_system(euler_override_system);
