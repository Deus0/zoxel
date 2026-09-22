// NOTE: This runs from a Node Process

// TODO: Tag the node so some can auto progress
// System handles the next node process
void next_node_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
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
            zox_add(e, TriggerBegin);
            zox_add(e, Dirty);
            if (dbg_log) {
                zox_log("Nodetree Progresses [%s] at [%s] to [%s]",
                    zox_sys_e_name,
                    zox_getn(previous),
                    zox_getn(next));
            }
        } else {
            // Finished Node Tree!
            zox_add(e, TriggerExit);
            if (dbg_log) {
                zox_log("Nodetree End [%s] at [%s]",
                    zox_sys_e_name,
                    zox_getn(previous));
            }
        }
    }
} zoxd_system(next_node_system);
