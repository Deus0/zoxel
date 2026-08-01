zox_sys2(Bounds3EnableSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Bounds3Dirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Bounds3Dirty, updated);
        if (updated->value == zox_dirty_active) {
            zox_remove(e, DisableMovement);
        }
    }
} zox_sys_end(Bounds3EnableSystem);
