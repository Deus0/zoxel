// todo: make use TargetChunkLod -> and load/unload depth based on that
// todo: set lod here instead of just applying RenderDepth only with BuildChunkMesh -> keeps memory down
// NOTE: This uses ChunkLod to increment the build per level
zox_sys2(CloneVoxSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CloneVoxLink);
    zox_sys_out(CloneVox);
    zox_sys_out(VoxelNode);
    zox_sys_out(NodeDepth);
    zox_sys_out(BlockScale);
    zox_sys_out(ChunkSize);
    zox_sys_out(ColorRGBs);
    zox_sys_out(VoxelNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CloneVoxLink, source);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(NodeDepth, ndepth);
        zox_sys_o(BlockScale, scale);
        zox_sys_o(ColorRGBs, colors);
        zox_sys_o(ChunkSize, csize);
        zox_sys_o(VoxelNodeDirty, dirty);
        zox_sys_o(CloneVox, state);
        entity src = source->value;
        if (!state->value || !src) {
            continue;
        }
        if (!zox_has(src, VoxelNode)) {
            continue;
        }
        // clone one depth at a time
        zox_geter(src, VoxelNode, soctree);
        zox_geter(src, NodeDepth, source_voctree_depth);
        zox_geter_value(src, BlockScale, float, src_scale);
        zox_geter(src, ChunkSize, source_chunk_size);
        zox_geter(src, ColorRGBs, colors_source);
        ndepth->value = source_voctree_depth->value;
        scale->value = src_scale;
        // Write Locks voctree
        write_lock_VoxelNode(voctree);
        // clone_at_depth_VoxelNode(voctree, soctree, clod->value, 0);
        clone_at_depth_VoxelNode(voctree, soctree, ndepth->value, 0);
        // Write Locks voctree
        write_unlock_VoxelNode(voctree);
        colors->length = colors_source->length;
        int clength = sizeof(color_rgb) * colors_source->length;
        resize_ColorRGBs(colors, clength);
        memcpy(colors->value, colors_source->value, clength);
        csize->value = source_chunk_size->value;
        state->value = 0;
        dirty->value = zox_dirty_trigger;
        // zox_log(" [%s] Is cloned from [%s] [%f]", zox_get_name(it->entities[i]), zox_get_name(src), scale->value);
    }
} zox_sys_end(CloneVoxSystem);
