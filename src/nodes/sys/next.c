// NOTE: This runs from a Node Process

// TODO: Tag the node so some can auto progress
// System handles the next node process
zox_sys2(NextNodeSystem) {
    byte dbg_log = 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeEnd);
    zox_sys_out(NodetreeEnd);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeEnd, state);
        zox_sys_o(NodetreeEnd, complete);
        // Process on completed single node
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity previous = zox_get_link(world, e, CurrentNodeLink);
        if (!previous) {
            continue;
        }
        entity next = zox_get_link(world, e, NextNodeLink);
        if (!next) {
            next = zox_get_link(world, previous, NodeLink);
        } else {
            zox_unlink(world, e, NextNodeLink, next);
            if (dbg_log) {
                zox_log("Consumed [NextNodeLink] [%s]",
                        zox_getn(next));
            }
        }
        if (previous == next) {
            zox_logw("Cannot progress to same node in tree [%s]",
                zox_sys_e_name);
            continue;
        }
        zox_unlink(world, e, CurrentNodeLink, previous);
        // Gets next node - First one for now
        if (next) {
            zox_link(world, e, CurrentNodeLink, next);
            zox_setv(e, NodeBegin, zox_dirty_trigger);
            zox_add(e, Dirty);
            if (dbg_log) {
                zox_log("Nodetree Progresses [%s] at [%s] to [%s]",
                    zox_sys_e_name,
                    zox_getn(previous),
                    zox_getn(next));
            }
        } else {
            // Finished Node Tree!
            complete->value = zox_dirty_trigger;
            if (dbg_log) {
                zox_log("Nodetree End [%s] at [%s]",
                    zox_sys_e_name,
                    zox_getn(previous));
            }
        }
    }
} zox_sys_end(NextNodeSystem);
