// todo: make use TargetChunkLod -> and load/unload depth based on that
// todo: set lod here instead of just applying RenderDepth only with BuildMesh -> keeps memory down
// NOTE: This uses ChunkLod to increment the build per level
void clone_vox_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(CloneVox);
    zox_sys_out(VoxelNode);
    zox_sys_out(NodeDepth);
    zox_sys_out(BlockScale);
    zox_sys_out(ChunkSize);
    zox_sys_out(ColorRGBs);
    // zox_sys_out(VoxelNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(NodeDepth, ndepth);
        zox_sys_o(BlockScale, scale);
        zox_sys_o(ColorRGBs, colors);
        zox_sys_o(ChunkSize, csize);
        // zox_sys_o(VoxelNodeDirty, dirty);
        zox_sys_o(CloneVox, state);
        entity src = zox_get_link(world, e, CloneVoxLink);
        if (!state->value || !src) {
            continue;
        }
        if (!zox_has(src, VoxelNode)) {
            continue;
        }
        // clone one depth at a time
        const VoxelNode* soctree = zox_get(src, VoxelNode);
        const NodeDepth* source_voctree_depth = zox_get(src, NodeDepth);
        float src_scale = zox_getv(src, BlockScale);
        const ChunkSize* source_chunk_size = zox_get(src, ChunkSize);
        const ColorRGBs* colors_source = zox_get(src, ColorRGBs);
        ndepth->value = source_voctree_depth->value;
        scale->value = src_scale;
        // its octree (written) and the source's (read) locked, address order (thread_safety.md)
        spinlock* other_locks[6] = { zox_octree_lock(world, src), NULL, NULL, NULL, NULL, NULL };
        spinlock* locked[7];
        const byte locked_count = zox_lock_voxel_trees(zox_octree_lock(world, e), other_locks, locked);
        clone_at_depth_VoxelNode(voctree, soctree, ndepth->value, 0);
        zox_unlock_voxel_trees(locked, locked_count);
        colors->length = colors_source->length;
        int clength = sizeof(color_rgb) * colors_source->length;
        resize_ColorRGBs(colors, clength);
        memcpy(colors->value, colors_source->value, clength);
        csize->value = source_chunk_size->value;
        state->value = 0;
        // dirty->value = zox_dirty_trigger;
        zox_add(e, VoxelNodeDirty);
        // zox_log(" [%s] Is cloned from [%s] [%f]", zox_get_name(it->entities[i]), zox_get_name(src), scale->value);
    }
    zox_sys_on_end();
} zoxd_system(clone_vox_system);
