zox_sys2(NodetreeBeginSystem) {
    byte dbg_log = 0;
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
        if (!zox_valid(current->value)) {
            // Finished Node Tree!
            zox_loge("Invalid Start Node in Tree [%s]",
                zox_sys_e_name);
            continue;
        }
        if (dbg_log) {
            zox_log("Node Graph Begin [%s] at [%s]",
                zox_sys_e_name,
                zox_getn(current->value));
        }
        zox_setv(e, NodeBegin, zox_dirty_trigger);
        // do this for now - then implement dialogue node system with confirm button
        // zox_set(e, NodeEnd, { zox_dirty_trigger });

    }
} zox_sys_end(NodetreeBeginSystem);
