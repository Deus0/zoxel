void test_arrayds(ecs* world, int n, int d) {
    zox_log("Running Arrayd Test:");
    zox_log("   Entities: %i", n);
    zox_log("   Datas: %i", d);
    zox_log("   Start ArrayDs: %i", zox_stats_arrayds_mallocs);
    zox_log("       - stat2: %i", zox_stats_arrayds_exists);
    entity entities[n];
    for (int i = 0; i < n; i++) {
        // create a node
        entity e = zox_new();
        Colors c = (Colors) { };
        initialize_Colors(&c, d);
        zox_set_ptr(e, Colors, c);
        entities[i] = e;
    }
    zox_log("   Peak ArrayDs: %i", zox_stats_arrayds_mallocs);
    zox_log("       - stat2: %i", zox_stats_arrayds_exists);

    // now destroy
    for (int i = 0; i < n; i++) {
        // create a node
        entity e = entities[i];
        zox_delete(e);
    }
    zox_log("   Dead ArrayDs: %i", zox_stats_arrayds_mallocs);
    zox_log("       - stat2: %i", zox_stats_arrayds_exists);
    zox_log("Exiting Zoxel after Test.");
    abort();
}