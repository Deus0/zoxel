//
void quantity_zero_event(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Quantity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Quantity, quantity);
        if (quantity->value == 0) {
            zox_delete(e);
        }
    }
}
