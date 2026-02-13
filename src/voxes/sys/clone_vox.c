// todo: make use TargetChunkLod -> and load/unload depth based on that
// todo: set lod here instead of just applying RenderDepth only with ChunkMeshDirty -> keeps memory down
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
    zox_sys_out(ChunkLod);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CloneVoxLink, cloneVoxLink);
        zox_sys_o(VoxelNode, node);
        zox_sys_o(NodeDepth, nodeDepth);
        zox_sys_o(BlockScale, scale);
        zox_sys_o(ColorRGBs, colorRGBs);
        zox_sys_o(ChunkSize, chunkSize);
        zox_sys_o(VoxelNodeDirty, nodeDirty);
        zox_sys_o(ChunkLod, chunkLod);
        zox_sys_o(CloneVox, cloneVox);

        entity src = cloneVoxLink->value;

        if (!cloneVox->value || !src) {
            continue;
        }

        if (!zox_has(src, VoxelNode)) {
            continue;
        }

        // clone one depth at a time
        zox_geter(src, VoxelNode, source_node);
        zox_geter(src, NodeDepth, source_node_depth);
        zox_geter_value(src, BlockScale, float, src_scale);
        zox_geter(src, ChunkSize, source_chunk_size);
        zox_geter(src, ColorRGBs, colors_source);

        if (chunkLod->value == 255) {
            chunkLod->value = 0;
        } else {
            chunkLod->value++;
        }

        nodeDepth->value = source_node_depth->value;
        scale->value = src_scale;

        // Write Locks node
        write_lock_VoxelNode(node);
        clone_at_depth_VoxelNode(node, source_node, chunkLod->value, 0);
        // Write Locks node
        write_unlock_VoxelNode(node);

        byte target_depth = nodeDepth->value;

        if (chunkLod->value == target_depth) {
            chunkSize->value = source_chunk_size->value;
            colorRGBs->length = colors_source->length;

            int memory_length = sizeof(color_rgb) * colors_source->length;
            initialize_ColorRGBs(colorRGBs, memory_length);
            memcpy(colorRGBs->value, colors_source->value, memory_length);

            cloneVox->value = 0;
            nodeDirty->value = zox_dirty_trigger;

            // zox_log(" [%s] Is cloned from [%s] [%f]", zox_get_name(it->entities[i]), zox_get_name(src), scale->value);
        }
    }
} zox_sys_end(CloneVoxSystem);
