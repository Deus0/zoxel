
zox_sys2(InitializeSystem) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_world();
    zox_sys_out(Initialize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(Initialize, state);
        if (state->value == zox_dirty_end) { // zox_dirty_end) {
            zox_remove(e, Initialize);
            if (dbg_log) {
                zox_log("Initialize? %i", zox_has(e, Initialize));
            }
            /*zprintf("owns=%d has=%d\n",
                ecs_owns(world, e, Initialize),
                ecs_has(world, e, Initialize));*/
        }
        state->value++;
    }
} zox_sys_end(InitializeSystem);
