// NOTE: Needs to use voxelt atm as it sets with air as default
// #define disable_newheightmap_gen
// place grass if max depth
// generates our terrain voxels
zox_sys2(RenderDepthChunk3System) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Loaded);
    zox_sys_in(RenderDepth);
    zox_sys_in(RenderDepthDirty);
    zox_sys_out(NodeDepth);
    zox_sys_out(Generate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Loaded, loaded);
        zox_sys_i(RenderDepth, render_depth);
        zox_sys_i(RenderDepthDirty, render_depth_dirty);
        zox_sys_o(NodeDepth, octree_depth);
        zox_sys_o(Generate, generate);
        if (render_depth_dirty->value != zox_dirty_active) {
            continue;
        }
        byte old_depth = octree_depth->value;
        // Rebuilds Mesh Whenever Render Depth is Dirty
        zox_set(e, VoxelNodeDirty, { zox_dirty_trigger });
        /*if (loaded->value) {
            zox_set(e, VoxelNodeGenerated, { zox_dirty_trigger });
        }*/
        // NOTE: This just updates the mesh
        // TODO: This is where we should set the chunk render LOD mesh
        if (loaded->value) {
            continue;
        }
        if (render_depth->value > old_depth) {
            // Set New Depth
            octree_depth->value = render_depth->value;
            if (dbg_log) {
                zox_log("Chunk [%s] Depth Increased -> %i", zox_get_name(e), render_depth->value);
            }
            // kicks off lighting if loaded
            if (!loaded->value) {
                // Set to Generate as Depth Increased
                generate->value = zox_dirty_trigger;
            }
        }
    }
} zox_sys_end(RenderDepthChunk3System);
