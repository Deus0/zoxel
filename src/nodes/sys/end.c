zox_sys2(NodetreeEndSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodetreeEnd);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodetreeEnd, end);
        if (end->value == zox_dirty_end) {
            zox_delete(e);
        }
    }
} zox_sys_end(NodetreeEndSystem);