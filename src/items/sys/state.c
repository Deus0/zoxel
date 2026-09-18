// Quantity Dirty Tag

void quantity_dirty_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, QuantityDirty);
    }
} zoxd_system(quantity_dirty_system);