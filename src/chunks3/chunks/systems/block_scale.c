void BlockScaleSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepthDirty);
    zox_sys_in(RenderDepth);
    zox_sys_in(VoxLink);
    zox_sys_out(BlockScale);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDepthDirty, dirty);
        zox_sys_i(RenderDepth, render_depth);
        zox_sys_i(VoxLink, vox_link);
        zox_sys_o(BlockScale, block_scale);
        if (dirty->value == zox_dirty_active && zox_valid(vox_link->value)) {
            zox_geter_value(vox_link->value, NodeDepth, byte, terrain_depth);
            zox_geter_value(vox_link->value, BlockScale, float, terrain_scalev);
            block_scale->value = get_chunk_scale(
                render_depth->value,
                terrain_depth,
                terrain_scalev
            );
        }
    }
} zoxd_system(BlockScaleSystem)
