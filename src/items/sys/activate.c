// extern void on_action_updated_quantity2(ecs*, const entity, const entity, const byte);
extern void on_action_removed(ecs*, entity, entity);

zox_sys2(ItemActivateSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Activate);
    zox_sys_in(UserLink);
    zox_sys_in(BlockLink);
    zox_sys_out(Quantity);
    zox_sys_out(QuantityDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Activate, activate);
        zox_sys_i(UserLink, user_link);
        zox_sys_i(BlockLink, block_link);
        zox_sys_o(Quantity, quantity);
        zox_sys_o(QuantityDirty, dirty);

        if (activate->value != zox_dirty_active) {
            continue;
        }

        if (!quantity->value) {
            continue;
        }

        entity user = user_link->value;
        zox_geter(user, RaycastVoxelData, raycast_data);
        zox_geter(user, RaycastRange, range);

        byte hit_block = raycast_data->result == rayhit_terrain;
        byte in_range = raycast_data->distance <= range->value;
        if (!hit_block || !in_range) {
            continue;
        }

        entity block = block_link->value;
        if (!zox_valid(block) || !zox_has(block, BlockIndex)) {
            zox_log_error("invalid block [%s]", zox_get_name(block));
            continue;
        }

        zox_geter_value(block, BlockIndex, byte, block_index);
        raycast_action(world, *raycast_data, block_index, 1);

        quantity->value--;

        if (!quantity->value) {
            // set action to nullptr
            // destroy entity
            on_action_removed(world, e, user);
            zox_delete(e);
        }

        // place block sound
        spawn_sound_generated(world,
            prefab_sound_generated,
            instrument_violin,
            note_frequencies[30 + rand() % 6],
            0.6,
            1.4f * get_volume_sfx()
        );

        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(ItemActivateSystem);
