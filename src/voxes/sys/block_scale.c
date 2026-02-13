// NOTE: Assumes node depth is lower than terrain (max depth)
static inline float get_chunk_scale(byte ndepth, byte tdepth, float tscale) {
    if (tdepth < ndepth) {
        zox_log_error("terrain < node: %i < %i", tdepth, ndepth);
        return tscale;
    }
    byte ddepth = tdepth - ndepth;
    return tscale * powers_of_two[ddepth];  // we multiply by the depth difference power - if 2 = 2*2 = 4, 0.5 becomes 2 in size
}

// Sync Block Scale to the Terrain Chunks from the Vox
zox_sys2(BlockScaleSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepthDirty);
    zox_sys_in(RenderDepth);
    zox_sys_in(VoxLink);
    zox_sys_out(BlockScale);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDepthDirty, dirty);
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_i(VoxLink, vox);
        zox_sys_o(BlockScale, scale);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (!zox_valid(vox->value) || !zox_has(vox->value, BlockScale)) {
            continue;
        }

        zox_geter_value(vox->value, NodeDepth, byte, tdepth);
        zox_geter_value(vox->value, BlockScale, float, terrain_scalev);

        scale->value = get_chunk_scale(rdepth->value, tdepth, terrain_scalev);

        // zox_log(" [%s] Scale set [%f]", zox_get_name(it->entities[i]), scale->value);
    }
} zox_sys_end(BlockScaleSystem);
