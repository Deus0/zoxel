//! Bounds, because everyone needs limits! Used atm to keep player within the start area.
zox_sys2(Bounds2DSystem) {
    zox_sys_begin();
    zox_sys_in(Bounds2D);
    zox_sys_out(Position2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Bounds2D, bounds2D);
        zox_sys_o(Position2, position2);
        if (position2->value.x > bounds2D->value.x)
            position2->value.x = bounds2D->value.x;
        else if (position2->value.x < -bounds2D->value.x)
            position2->value.x = -bounds2D->value.x;
        if (position2->value.y > bounds2D->value.y)
            position2->value.y = bounds2D->value.y;
        else if (position2->value.y < -bounds2D->value.y)
            position2->value.y = -bounds2D->value.y;
    }
} zox_sys_end(Bounds2DSystem);
