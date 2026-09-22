void node_start_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // get start link
        entity start_node = zox_get_link(world, e, NodeLink);
        if (!zox_valid(start_node)) {
            // Finished Node Tree!
            zox_loge("Invalid Start Node in Tree [%s]",
                zox_sys_e_name);
            continue;
        }
        // Unlink Previous
        entity previous = zox_get_link(world, e, CurrentNodeLink);
        if (previous) {
            zox_unlink(world, e, CurrentNodeLink, previous);
        }
        zox_link(world, e, CurrentNodeLink, start_node);
        zox_add(e, TriggerBegin);
        zox_add(e, Dirty);
        if (dbg_log) {
            zox_log("Nodetree [%s] Begins at [%s]",
                zox_sys_e_name,
                zox_getn(start_node));
        }
    }
} zoxd_system(node_start_system);
