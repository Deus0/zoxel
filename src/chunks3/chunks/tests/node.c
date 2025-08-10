void test_nodes(ecs* world, int n, int d) {
    zox_log("Running Node Test:");
    zox_log("   N: %i", n);
    zox_log("   Depth: %i", d);
    zox_log("   Start Nodes: %i", zox_stats_nodes);
    entity entities[n];
    for (int i = 0; i < n; i++) {
        // create a node
        entity e = zox_new();
        VoxelNode node = (VoxelNode) { };
        random_fill_octree(&node, 1, d);
        zox_set_ptr(e, VoxelNode, node);
        entities[i] = e;
    }
    zox_log("   Peak Nodes: %i", zox_stats_nodes);
    // now destroy
    for (int i = 0; i < n; i++) {
        // create a node
        entity e = entities[i];
        zox_delete(e);
    }
    zox_log("   Dead Nodes: %i", zox_stats_nodes);
}