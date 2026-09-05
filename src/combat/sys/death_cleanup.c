extern double zox_current_time;

// time after death, character will be removed
zox_sys2(DeathCleanSystem) {
    double cleanup_time = 2.0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DiedTime);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DiedTime, time);
        if (zox_current_time - time->value >= cleanup_time) {
            entity camera = zox_get_link(world, e, Camera);
            if (zox_valid(camera)) {
                zox_set_parent(world, camera, 0);
            }
            zox_delete(e);
        }
    }
} zox_sys_end(DeathCleanSystem);
