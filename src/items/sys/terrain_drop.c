// SpawnS a pickup when removed
zox_sys2(TerrainItemDropSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeQueue);
    zox_sys_in(VoxelNode);
    zox_sys_in(VoxLink);
    zox_sys_in(NodeDepth);
    zox_sys_in(Position3D);
    zox_sys_in(BlockScale);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxelNodeQueue, queue);
        zox_sys_i(VoxelNode, node);
        zox_sys_i(VoxLink, link);
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(Position3D, position);
        zox_sys_i(BlockScale, scale);

        if (!queue->count) {
            continue;
        }

        zox_geter_value(link->value, RealmLink, entity, realm);
        if (!zox_valid(realm)) {
            continue;
        }

        zox_geter(realm, BlockLinks, blocks);

        for (size_t j = 0; j < queue->count; j++) {
            VoxelNodeUpdate update = queue->ptr[j];

            byte voxel = get_value_VoxelNode(
                node,
                depth->value,
                update.pos,
                0);

            if (!voxel) {
                continue;
            }

            entity block = blocks->value[voxel - 1];

            if (!zox_has(block, ItemLink)) {
                zox_log_error("block [%s] has no ItemLink", zox_get_name(block));
                continue;
            }

            zox_geter_value(block, ItemLink, entity, block_item);
            if (!zox_valid(block_item)) {
                zox_log_error("block [%s] has no valid item", zox_get_name(block));
            }

            float3 positionf = byte3_to_float3(update.pos);
            float3_scale_p(&positionf, scale->value);
            float3_add_float3_p(&positionf, position->value); // chunk
            float3_add_float3_p(&positionf, float3_single(scale->value * 0.5f));

            // zox_log("Spawned block pickup at [%fx%fx%f] scale [%f]", positionf.x, positionf.y, positionf.z, scale->value);
            // get positionf from local position and depth

            entity pickup = spawn_pickup_block(world, positionf, block);
            if (pickup) {
                zox_set(pickup, ItemLink, { block_item });
            }
        }
    }
} zox_sys_end(TerrainItemDropSystem);
