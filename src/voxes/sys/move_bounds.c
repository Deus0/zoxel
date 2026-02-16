zox_sys2(Bounds3EnableSystem) {
    zox_sys_begin();
    zox_sys_in(Bounds3Dirty);
    zox_sys_out(DisableMovement); zox_sys_out(DisableGravity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Bounds3Dirty, updated);
        zox_sys_o(DisableMovement, disabled);
        zox_sys_o(DisableGravity, disabled2);

        if (updated->value != zox_dirty_active) {
            continue;
        }

        // TODO: use init instead of dirty state

        disabled->value = 0;
        disabled2->value = 0;
    }
} zox_sys_end(Bounds3EnableSystem);
