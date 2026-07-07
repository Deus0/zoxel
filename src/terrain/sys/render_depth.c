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
            }
        }
        // NOTE: Rebuilds Mesh Whenever Render Depth is Dirty, unless generating
        if (!generate->value) {
            zox_set(e, VoxelNodeDirty, { zox_dirty_trigger });
        }
    }
} zox_sys_end(RenderDepthChunk3System);
