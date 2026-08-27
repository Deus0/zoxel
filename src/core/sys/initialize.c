// NOTE: Increments Initialize, then removes it
void initialize_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    // zox_sys_out(Initialize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        /*zox_sys_o(Initialize, state);
        if (state->value < zox_dirty_end) {
            state->value++;
        } else {
        }*/
        zox_remove(e, Initialize);
        if (dbg_log) {
            zox_log("Initialize Removed [%s]",
                zox_getn(e));
        }
    }
    zox_sys_on_end();
} zoxd_system(initialize_system);

void pre_initialize_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, PreInitialize);
        zox_add(e, Initialize);
        if (dbg_log) {
            zox_log("PreInitialize Removed [%s]",
                zox_getn(e));
        }
    }
    zox_sys_on_end();
} zoxd_system(pre_initialize_system);
