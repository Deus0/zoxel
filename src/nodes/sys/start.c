zox_sys2(NodetreeBeginSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeStartLink);
    zox_sys_in(NodetreeBegin);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeStartLink, start);
        zox_sys_i(NodetreeBegin, state);
        if (state->value != zox_dirty_active) {
            continue;
        }
        // Unlink Previous
        entity previous = zox_get_link(world, e, CurrentNodeLink);
        if (previous) {
            zox_unlink(world, e, CurrentNodeLink, previous);
        }
        entity node = start->value;
        if (!zox_valid(node)) {
            // Finished Node Tree!
            zox_loge("Invalid Start Node in Tree [%s]",
                zox_sys_e_name);
            continue;
        }
        zox_link(world, e, CurrentNodeLink, node);
        zox_setv(e, NodeBegin, zox_dirty_trigger);
        if (dbg_log) {
            zox_log("Nodetree [%s] Begins at [%s]",
                zox_sys_e_name,
                zox_getn(node));
        }
        // do this for now - then implement dialogue node system with confirm button
        // zox_set(e, NodeEnd, { zox_dirty_trigger });

    }
} zox_sys_end(NodetreeBeginSystem);
