// NOTE: This runs from a Node Process
// System handles the next node process
/*zox_sys2(NodeBeginEventSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeBegin);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeBegin, state);
        if (state->value == zox_dirty_active) {
            continue;
        }
        entity current = zox_get_link(world, e, CurrentNodeLink);
        if (!current ||
            !zox_has(current, NodeBeginEvent))
        {
            continue;
        }
        const NodeBeginEvent* event = zox_get(current, NodeBeginEvent);
        for (int j = 0; j < event->count; j++) {
            if (event->value[j].fun == NULL) {
                zox_loge("NodeBeginEvent [%s] Null [%i]", zox_getn(current), j);
                continue;
            }
            if (dbg_log) {
                zox_log("Calling NodeBeginEvent [%i] ptr [%p]", j, (void *) event->value[j].fun);
            }
            // NOTE: Pass in World, Process and Node
            event->value[j].fun(
                world,
                e,
                current);
        }
    }
} zox_sys_end(NodeBeginEventSystem);*/
