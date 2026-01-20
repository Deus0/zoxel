zox_sys2(NodetreeBeginSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeStartLink);
    zox_sys_in(NodetreeBegin);
    zox_sys_out(NodeLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeStartLink, start);
        zox_sys_i(NodetreeBegin, state);
        zox_sys_o(NodeLink, current);

        if (state->value != zox_dirty_active) {
            continue;
        }

        current->value = start->value;

        zox_logv("Node Graph Begin [%s]", zox_get_name(e));

        if (!zox_valid(current->value)) {
            // Finished Node Tree!
            // complete->value = zox_dirty_trigger;
            // zox_log("Dialogue Completed.");
            // zox_logw("Node Process has no Tree.", zox_get_name(e));
            continue;
        }

        zox_set(e, NodeBegin, { zox_dirty_trigger });
        // do this for now - then implement dialogue node system with confirm button
        // zox_set(e, NodeEnd, { zox_dirty_trigger });

    }
} zox_sys_end(NodetreeBeginSystem);