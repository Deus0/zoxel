//! Each entity will impact all other entities.
zox_sys2(Gravity2DSystem) {
    // NearbyEntities component instead of everything.
    // double delta_time = (double) it->delta_time;
    zox_sys_begin();
    zox_sys_out(Position2);
    zox_sys_in(Velocity2D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Position2, position2);
        zox_sys_i(Velocity2D, velocity2D);
        position2->value.x += velocity2D->value.x * zox_delta_time;
        position2->value.y += velocity2D->value.y * zox_delta_time;
    }
} zox_sys_end(Gravity2DSystem);