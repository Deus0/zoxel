zox_sys2(InsideBlockSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Bounds3D);
    zox_sys_out(InsideBlock);
    zox_sys_out(InsideBlockPosition);
    zox_sys_out(InsideBlockDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(Bounds3D, bounds);
        zox_sys_o(InsideBlock, inside);
        zox_sys_o(InsideBlockPosition, iposition);
        zox_sys_o(InsideBlockDirty, idirty);
        entity terrain = zox_get_link(world, e, TerrainLink);
        if (!zox_valid(terrain)) {
            zox_loge("Terrain invalid in character [%s]", zox_get_name(e));
            continue;
        }
        zox_geter(terrain, ChunkLinks, chunks);
        zox_geter_value(terrain, BlockScale, float, terrain_scale);
        zox_geter_value(terrain, NodeDepth, byte, terrain_depth);
        float3 positionf = float3_add(position->value,
            (float3) { 0, - bounds->value.y / 4.0f, 0 });
        int3 positionv = real_position_to_block_position(positionf, terrain_scale);
        byte length = octree_size(terrain_depth);
        byte3 max_chunk_size = byte3_single(length);
        int3 positionc = block_position_to_positionc(positionv, max_chunk_size);
        // get points chunk
        entity chunk = int3_hashmap_get(chunks->value, positionc);
        // If not in terrain chunk
        if (!zox_valid(chunk)) {
            continue;
        }
        if (!zox_has(chunk, VoxelNode) || !zox_has(chunk, NodeDepth)) {
            zox_log_error("Invalid [%s] Chunk Components [%lu]", zox_get_name(chunk), chunk);
            continue;
        }
        zox_geter(chunk, VoxelNode, voctree);
        zox_geter_value(chunk, NodeDepth, byte, cdepth);
        byte length2 = octree_size(cdepth);
        byte3 csize = byte3_single(length2);
        byte3 positionl = get_positionl_byte3(positionv, csize);
        if (!byte3_in_bounds(positionl, csize)) {
            zox_log_error("Voxel OOB: [%ix%ix%i] :: %i", positionl.x, positionl.y, positionl.z, csize.x);
            continue;
        }
        // voxel
        byte voxel = getv_VoxelNode(voctree, cdepth, positionl);
        // byte voxel = get_sub_node_voxel_locked(voctree, &positionl, cdepth);
        if (inside->value != voxel) {
            inside->value = voxel;
            idirty->value = zox_dirty_trigger;
            // zox_log("Inside new block [%i]", voxel);
        } else if (!int3_equals(positionv, iposition->value)) {
            // Compares voxel position and triggers when moving into same block type
            idirty->value = zox_dirty_trigger;
        }
        iposition->value = positionv;
    }
} zox_sys_end(InsideBlockSystem);
