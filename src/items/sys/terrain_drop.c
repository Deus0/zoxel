// SpawnS a pickup when removed
// TODO: This has 17ms try optimize it?
zox_sys2(TerrainItemDropSystem) {
    byte dbg_log = 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelDropQueue);
    zox_sys_in(Position3D);
    zox_sys_in(BlockScale);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelDropQueue, drops);
        zox_sys_i(Position3D, position);
        zox_sys_i(BlockScale, scale);
        if (!drops->count) {
            continue;
        }
#ifdef zox_safety_checks
        if (!scale->value) {
            zox_loge("Block Scale 0 in chunk [%s]", zox_getn(e));
            continue;
        }
#endif
        entity terrain = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_loge("Terrain Invalid [TerrainItemDropSystem]");
            continue;
        }
#endif
        entity realm = zox_get_parent(world, terrain);
#ifdef zox_safety_checks
        if (!zox_valid(realm)) {
            zox_loge("Realm Invalid [TerrainItemDropSystem]");
            continue;
        }
#endif
        const BlockLinks* blocks = zox_get(realm, BlockLinks);
        for (size_t j = 0; j < drops->count; j++) {
            VoxelDropElement update = drops->ptr[j];
            byte voxel = update.value;
            if (!voxel) {
                zox_loge("Voxel Dropped is Air at [%ix%ix%i]", update.pos.x, update.pos.y, update.pos.z);
                continue;
            }
            entity block = blocks->value[voxel - 1];
            if (!zox_valid(block)) {
                zox_loge("TerrainDrop: Block Invalid [%i]", voxel - 1);
                continue;
            }
            if (!zox_has(block, ItemLink)) {
                zox_log_error("block [%s] has no ItemLink", zox_getn(block));
                continue;
            }
            entity block_item = zox_getv(block, ItemLink);
            if (!zox_valid(block_item)) {
                zox_log_error("block [%s] has no valid item", zox_get_name(block));
            }
            float3 positionf = byte3_to_float3(update.pos);
            float3_scale_p(&positionf, scale->value);
            float3_add_float3_p(&positionf, position->value); // chunk
            float3_add_float3_p(&positionf, float3_single(scale->value * 0.5f));
            // get positionf from local position and depth
            entity pickup = spawn_pickup_block(world, positionf, block, item_pickup_scale);
            if (pickup) {
                zox_set(pickup, ItemLink, { block_item });
            }
            if (dbg_log) {
                zox_log("Spawned block pickup at [%fx%fx%f] scale [%f]", positionf.x, positionf.y, positionf.z, scale->value);
            }
        }
    }
} zox_sys_end(TerrainItemDropSystem);
