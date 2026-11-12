// Quantity Dirty Tag

void quantity_dirty_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, QuantityDirty);
    }
    zox_sys_on_end();
} zoxd_system(quantity_dirty_system);