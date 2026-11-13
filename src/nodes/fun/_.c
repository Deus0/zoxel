void delete_nodes(ecs* world, entity e) {
    if (!zox_valid(e)) {
        return;
    }
    uint link_index = 0;
    while (1) {
        entity node = zox_get_link_index(
            world,
            e,
            NodeLink,
            link_index);
        if (!node) {
            break;
        }
        delete_nodes(world, node);
        link_index++;
    }
    /*if (zox_has(e, NodeLinks)) {
        const NodeLinks* nodes = zox_get(e, NodeLinks);
        for (int i = 0; i < nodes->length; i++) {
            entity e2 = nodes->value[i];
            delete_nodes(world, e2);
        }
    }*/
    zox_delete(e);
}

// links a node to its next, and makes the next its child: graphs are trees built forward (each node linked
//  once, by the one before it), so deleting a graph's root (a child of its realm or model) deletes it all
//  - only the root was a child before, the rest leaked when a game ended (tst end_game_cleanup)
void new_link_single_node(ecs* world, entity e, entity e2) {
    zox_link(world, e, NodeLink, e2);
    zox_set_parent_non_fragment(world, e2, e);
    // NodeLinks links = (NodeLinks) { 0 };
    // add_to_NodeLinks(&links, e2);
    // zox_set_ptr(e, NodeLinks, links);
}

// Used for conditional nodes atm
void delayed_node_end(ecs* world, entity node) {
    zox_add(node, TriggerEnd);
}
