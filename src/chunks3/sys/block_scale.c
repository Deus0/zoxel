// add here
static inline float get_chunk_scale(
    byte node_depth,
    byte terrain_depth,
    float terrain_voxel_scale
) {
    if (terrain_depth < node_depth) {
        zox_log_error("terrain < node: %i < %i", terrain_depth, node_depth);
        return terrain_voxel_scale;
    }
    byte ddepth = terrain_depth - node_depth;   // assuming node depth is lower than terrain (max depth)
    return terrain_voxel_scale * powers_of_two[ddepth];  // we multiply by the depth difference power - if 2 = 2*2 = 4, 0.5 becomes 2 in size
}

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

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (!zox_valid(vox_link->value) || !zox_has(vox_link->value, BlockScale)) {
            continue;
        }

        zox_geter_value(vox_link->value, NodeDepth, byte, terrain_depth);
        zox_geter_value(vox_link->value, BlockScale, float, terrain_scalev);
        block_scale->value = get_chunk_scale(
            render_depth->value,
            terrain_depth,
            terrain_scalev
        );
        // zox_log(" [%s] Scale set [%f]", zox_get_name(it->entities[i]), block_scale->value);
    }
} zoxd_system2(BlockScaleSystem);
