// NOTE: Veloco Rapters!
void velocity3_system(iter* it) {
    if (zox_delta_time >= zox_physics_max_delta_time) {
        return;
    }
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_in(Velocity3D);
    zox_sys_out(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Velocity3D, velocity);
        zox_sys_o(Position3D, position);
        position->value = float3_add(position->value, float3_scale(velocity->value, zox_delta_time));
    }
    zox_sys_on_end();
} zoxd_system(velocity3_system);
