// Set streamer data from the terrain
zox_sys2(StreamPointSyncSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StreamLink);
    zox_sys_out(BlockScale);
    zox_sys_out(NodeDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(StreamLink, terrain);
        zox_sys_o(BlockScale, scale);
        zox_sys_o(NodeDepth, depth);

        if (!zox_valid(terrain->value) || !zox_has(terrain->value, BlockScale) || !zox_has(terrain->value, NodeDepth)) {
            continue;
        }

        zox_geter_value(terrain->value, BlockScale, float, tscale);
        zox_geter_value(terrain->value, NodeDepth, byte, ndepth);

        scale->value = tscale;
        depth->value = ndepth;

        // zox_log("Syncing stream poiint");
    }
} zox_sys_end(StreamPointSyncSystem);