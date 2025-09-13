void NextNodeSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeEnd);
    zox_sys_out(NodeLink);
    zox_sys_out(NodetreeEnd);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeEnd, state);
        zox_sys_o(NodeLink, current);
        zox_sys_o(NodetreeEnd, complete);

        // Process on completed single node
        if (!zox_valid(current->value) || state->value != zox_dirty_active) {
            continue;
        }

        // Gets next node - First one for now
        zox_geter(current->value, NodeLinks, nodes);
        current->value = nodes->length > 0 ? nodes->value[0] : 0;

        if (!zox_valid(current->value)) {
            // Finished Node Tree!
            complete->value = zox_dirty_trigger;
            // zox_log("Dialogue Completed.");
            zox_log("Nodetree End [%s]", zox_get_name(e));
        } else {
            zox_set(e, NodeBegin, { zox_dirty_trigger });
            // do this for now - then implement dialogue node system with confirm button
            // zox_set(e, NodeEnd, { zox_dirty_trigger });
        }

    }
} zoxd_system2(NextNodeSystem);