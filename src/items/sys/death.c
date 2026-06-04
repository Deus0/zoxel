// NOTE: This destroys items when they run out!
zox_sys2(ItemQuantityDeathSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(QuantityDirty);
    zox_sys_in(Quantity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(QuantityDirty, dirty);
        zox_sys_i(Quantity, quantity);
        if (dirty->value == zox_dirty_active) {
            if (quantity->value == 0) {
                // zox_log("Item ran out [%s]", zox_get_name(e));
                zox_delete(e);
            }
        }
    }
} zox_sys_end(ItemQuantityDeathSystem);
