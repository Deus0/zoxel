extern double zox_current_time;
double cleanup_time = 2.0;
// time after death, character will be removed
zox_sys2(DeathCleanSystem) {
    double time = zox_current_time;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Dead);
    zox_sys_in(DiedTime);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Dead, dead);
        zox_sys_i(DiedTime, diedTime);
        if (dead->value != zox_dirty_end) {
            continue;
        }
        if (time - diedTime->value >= cleanup_time) {

            if (zox_has(e, CameraLink)) {
                zox_geter_value(e, CameraLink, entity, camera);
                zox_set_parent(world, camera, 0);
            }

            zox_delete(e);
        }
    }
} zox_sys_end(DeathCleanSystem);
