// Increments the Initialize, then removes it
zox_sys2(InitializeSystem) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_world();
    zox_sys_out(Initialize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(Initialize, initialize);
        if (initialize->value < zox_dirty_end) {
            initialize->value++;
        } else {
#ifndef zox_disable_initialize_removal
            zox_remove(e, Initialize);
#endif
            if (dbg_log) {
                zox_log("Initialize Removed [%s]", zox_getn(e));
            }
        }
    }
} zox_sys_end(InitializeSystem);

/*zprintf("owns=%d has=%d\n",
    ecs_owns(world, e, Initialize),
    ecs_has(world, e, Initialize));*/
