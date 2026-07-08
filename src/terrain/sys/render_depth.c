// NOTE: When Depth Increases, we set to GenerateChunk
zox_sys2(RenderDepthChunk3System) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Loaded);
    zox_sys_in(RenderDepth);
    zox_sys_in(RenderDepthDirty);
    zox_sys_out(NodeDepth);
    zox_sys_out(GenerateChunk);
    zox_sys_out(Busy);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Loaded, loaded);
        zox_sys_i(RenderDepth, render_depth);
        zox_sys_i(RenderDepthDirty, render_depth_dirty);
        zox_sys_o(NodeDepth, octree_depth);
        zox_sys_o(GenerateChunk, generate);
        zox_sys_o(Busy, busy);
        if (render_depth_dirty->value != zox_dirty_active) {
            continue;
        }
        busy->value = 1;
        // NOTE: This just updates the mesh
        // TODO: This is where we should set the chunk render LOD mesh
        if (render_depth->value > octree_depth->value) {
            // Set New Depth
            octree_depth->value = render_depth->value;
            if (dbg_log) {
                zox_log("Chunk [%s] Depth Increased -> %i", zox_get_name(e), render_depth->value);
            }
            // Set to Generate as Depth Increased
            if (!loaded->value) {
                generate->value = zox_generate_tchunk_start;
                // When Depth Increases, Clear Lights
                // NOTE: When clears, it would need to reflood the side lights, if sunlight rebeaming them
                /*zox_muter(e, LightNode, lights);
                lights->value = 0;
                collapse_LightNode(lights);*/
            } else {
                // Skips Generation
                generate->value = zox_generate_tchunk_end;
            }
        } else {
            // NOTE: Rebuilds Mesh Whenever Render Depth is Dirty, unless generating
            zox_set(e, VoxelNodeDirty, { zox_dirty_trigger });
        }
    }
} zox_sys_end(RenderDepthChunk3System);
