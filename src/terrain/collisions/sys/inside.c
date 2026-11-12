void inside_block_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Bounds3D);
    zox_sys_out(InsideBlock);
    zox_sys_out(InsideBlockPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(Bounds3D, bounds);
        zox_sys_o(InsideBlock, inside);
        zox_sys_o(InsideBlockPosition, iposition);
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
            zox_loge("Invalid [%s] Chunk Components [%lu]", zox_get_name(chunk), chunk);
            continue;
        }
        zox_geter(chunk, VoxelNode, voctree);
        zox_geter_value(chunk, NodeDepth, byte, cdepth);
        byte length2 = octree_size(cdepth);
        byte3 csize = byte3_single(length2);
        byte3 positionl = get_positionl_byte3(positionv, csize);
        if (!byte3_in_bounds(positionl, csize)) {
            zox_loge("Voxel OOB: [%ix%ix%i] :: %i", positionl.x, positionl.y, positionl.z, csize.x);
            continue;
        }
        // voxel
        spinlock* chunk_lock = zox_octree_lock(world, chunk);   // writers can run alongside (thread_safety.md)
        zox_octree_lock_begin(chunk_lock);
        byte voxel = getv_VoxelNode(voctree, cdepth, positionl);
        zox_octree_lock_end(chunk_lock);
        // byte voxel = get_sub_node_voxel_locked(voctree, &positionl, cdepth);
        if (inside->value != voxel) {
            inside->value = voxel;
            iposition->value = positionv;
            // zox_log("Inside new block [%i]", voxel);
        } else if (!int3_equals(positionv, iposition->value)) {
            // Compares voxel position and triggers when moving into same block type
            iposition->value = positionv;
        } else {
            continue;
        }
        zox_add(e, InsideBlockDirty);
    }
    zox_sys_on_end();
} zoxd_system(inside_block_system);
