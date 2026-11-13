// Sync Block Scale to the Terrain Chunks from the Vox
void block_scale_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepthDirty);
    zox_sys_in(RenderDepth);
    zox_sys_out(BlockScale);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepthDirty, dirty);
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_o(BlockScale, scale);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain) || !zox_has(terrain, BlockScale)) {
            continue;
        }
        byte tdepth = zox_getv(terrain, NodeDepth);
        float terrain_scalev = zox_getv(terrain, BlockScale);
        scale->value = get_chunk_scale(rdepth->value, tdepth, terrain_scalev);
        // zox_log(" [%s] Scale set [%f]", zox_get_name(it->entities[i]), scale->value);
    }
    zox_sys_on_end();
} zoxd_system(block_scale_system);
