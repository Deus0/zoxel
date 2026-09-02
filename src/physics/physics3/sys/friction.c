float ground_friction3 = 6;
float air_friction3 = 1;

void friction3_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_in(Grounded);
    zox_sys_in(Velocity3D);
    zox_sys_out(Acceleration3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Grounded, grounded);
        zox_sys_i(Velocity3D, velocity);
        zox_sys_o(Acceleration3D, acceleration);
        float friction_power = grounded->value ?
            ground_friction3 :
            air_friction3;
        if (friction_power != 0) {
            acceleration->value = float3_add(
                acceleration->value,
                float3_scale(
                    velocity->value,
                    -friction_power));
        }
    }
    zox_sys_on_end();
} zoxd_system(friction3_system);
