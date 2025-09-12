void NodetreeBeginSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodetreeLink);
    zox_sys_in(NodetreeBegin);
    zox_sys_out(NodeLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodetreeLink, start);
        zox_sys_i(NodetreeBegin, state);
        zox_sys_o(NodeLink, current);

        if (state->value != zox_dirty_active) {
            continue;
        }

        current->value = start->value;
        zox_log("Nodetree Begin [%s]", zox_get_name(e));

        if (!zox_valid(current->value)) {
            // Finished Node Tree!
            // complete->value = zox_dirty_trigger;
            // zox_log("Dialogue Completed.");
            zox_logw("Node Process has no Tree.", zox_get_name(e));
        } else {
            zox_set(e, NodeBegin, { zox_dirty_trigger });
            // do this for now - then implement dialogue node system with confirm button
            zox_set(e, NodeEnd, { zox_dirty_trigger });
        }

    }
} zoxd_system2(NodetreeBeginSystem);