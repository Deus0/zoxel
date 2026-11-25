// tst: node_graph_cleanup, a graph owner deletes all of its branches and clears incoming links from surviving nodes.
static void zox_tst_node_graph_cleanup_check(ecs* world) {
    const entity owner = zox_new();
    const entity root = spawn_node_leaf(world, prefab_node);
    const entity first = spawn_node_leaf(world, prefab_node);
    const entity second = spawn_node_leaf(world, prefab_node);
    const entity tip = spawn_node_leaf(world, prefab_node);
    const entity external = spawn_node_leaf(world, prefab_node);
    const entity survivor = spawn_node_leaf(world, prefab_node);
    zox_set_parent_non_fragment(world, root, owner);
    new_link_single_node(world, root, first);
    new_link_single_node(world, root, second);
    new_link_single_node(world, second, tip);
    zox_link(world, root, NodeLink, external);
    zox_link(world, external, NodeLink, tip);
    zox_link(world, external, NodeLink, survivor);
    zox_tst_check(zox_get_parent(world, first) == root && zox_get_parent(world, second) == root && zox_get_parent(world, tip) == second,
        "linking nodes did not establish graph ownership");
    zox_defer_begin();
    zox_delete(owner);
    zox_defer_end();
    zox_tst_check(!zox_valid(root) && !zox_valid(first) && !zox_valid(second) && !zox_valid(tip), "deleting the graph owner left owned nodes alive");
    zox_tst_check(zox_valid(external) && zox_valid(survivor), "graph deletion followed a link outside its ownership tree");
    zox_tst_check(zox_get_link(world, external, NodeLink) == survivor && !zox_get_link_index(world, external, NodeLink, 1),
        "graph deletion did not clear just the incoming link to its removed leaf");
}
