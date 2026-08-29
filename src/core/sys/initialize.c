// NOTE: Increments Initialize, then removes it
void initialize_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Initialize);
        if (dbg_log) {
            zox_log("Initialize Removed [%s]",
                zox_sys_e_name);
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
                zox_sys_e_name);
        }
    }
    zox_sys_on_end();
} zoxd_system(pre_initialize_system);
