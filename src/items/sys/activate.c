// NOTE: This assumes item quantity!
zox_sys2(ItemActivateSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Activate);
    // zox_sys_in(BlockLink);
    zox_sys_out(Quantity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Activate, activate);
        // zox_sys_i(BlockLink, block_link);
        zox_sys_o(Quantity, quantity);
        if (activate->value != zox_dirty_active ||
            !quantity->value)
        {
            continue;
        }
        entity user = zox_get_parent(world, e);
        zox_geter(user, RaycastVoxelData, raycast_data);
        zox_geter(user, RaycastRange, range);
        byte hit_block = raycast_data->result == rayhit_terrain;
        byte in_range = raycast_data->distance <= range->value;
        if (!hit_block ||
            !in_range)
        {
            continue;
        }
        // entity block = block_link->value;
        entity block = zox_get_link(world, e, BlockLink);
        if (!zox_valid(block) ||
            !zox_has(block, BlockIndex))
        {
            zox_loge("invalid block [%s]", zox_get_name(block));
            continue;
        }
        byte block_index = zox_getv(block, BlockIndex);
        // Place Block
        byte3 positionl = raycast_data->positionl_last;
        entity chunk = raycast_data->chunk_last;
        if (!zox_valid(chunk) ||
            !zox_has(chunk, VoxelNodeQueue)
        ) {
            zox_loge("Invalid rayhit chunk on user [%s]",
                zox_getn(user));
            continue;
        }
        zox_muter(chunk, VoxelNodeQueue, queue);
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) {
            .value = block_index,
            .position = positionl
        });
        quantity->value--;
        zox_add(e, QuantityDirty);
        // place block sound
        spawn_sound_generated(
            world,
            prefab_sound_generated,
            instrument_violin,
            note_frequencies[30 + rand() % 6],
            0.6,
            1.4f * get_volume_sfx());
        if (zox_has(user, SwingStart)) {
            float swing_time =
                zox_getv(e, WarmupTime) +
                zox_getv(e, CooldownTime);
            zox_setv(user, SwingStart, zox_current_time);
            zox_setv(user, SwingSpeed, swing_time);
        }
    }
} zox_sys_end(ItemActivateSystem);
