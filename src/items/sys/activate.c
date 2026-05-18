// NOTE: This assumes item quantity!
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
        // Place Block
        byte3 positionl = raycast_data->positionl_last;
        entity chunk = raycast_data->chunk_last;
        zox_muter(chunk, VoxelNodeQueue, queue);
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = block_index, .pos = positionl });
        quantity->value--;
        // place block sound
        spawn_sound_generated(world, prefab_sound_generated, instrument_violin, note_frequencies[30 + rand() % 6], 0.6, 1.4f * get_volume_sfx());
        dirty->value = zox_dirty_trigger;
        // Deletes the item itself
        /*if (!quantity->value) {
            zox_delete(e);
        }*/
    }
} zox_sys_end(ItemActivateSystem);

            // set action to nullptr
            // destroy entity
            // on_action_removed(world, e, user);
            /*entity actionbar = zox_get_child_by_id(world, user, zox_id(Actionbar));
            if (zox_valid(actionbar)) {
                entity item_slot = zox_get_slot_by_data(world, actionbar, e);
                if (zox_valid(item_slot)) {
                    zox_muter(item_slot, DataLink, slot_data);
                    if (slot_data->value) {
                        slot_data->value = 0;
                        zox_set(item_slot, DataDirty, { zox_dirty_active });
                    }
                } else {
                    zox_loge("Could not find item in actionbar slots [%s]", zox_get_name(e));
                    zox_print_slots(world, actionbar);
                }
            }*/
// extern void on_action_updated_quantity2(ecs*, const entity, const entity, const byte);
/*void on_action_removed(ecs* world, entity e, entity user) {
    zox_muter(user, ActionLinks, actions);
    int action_index = -1;
    for (int j = 0; j < actions->length; j++) {
        if (actions->value[j] == e) {
            action_index = j;
            break;
        }
    }
    if (action_index == -1) {
        return;
    }
    actions->value[action_index] = 0;
    if (!zox_has(user, ElementLinks)) {
        zox_log_error("Character has no ElementLinks");
        return;
    }
    zox_geter(user, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuActions, actionbar);
    if (!zox_valid(actionbar)) {
        zox_log_error("Character has no actionbar");
        return;
    }
    entity actionbar_children[layouts2_children_capacity];
    uint actionbar_children_length = zox_get_children(world, actionbar, actionbar_children, layouts2_children_capacity);
    if (actionbar_children_length < 2) {
        zox_log_error("Character has bad menu_actions_children");
        return;
    }
    entity body = actionbar_children[1];
    if (!zox_valid(body)) {
        zox_log_error("invalid menu_actions_body")
        return;
    }
    entity body_children[layouts2_children_capacity];
    uint body_children_length = zox_get_children(world, body, body_children, layouts2_children_capacity);
    if (action_index >= body_children_length) {
        return;
    }
    entity frame = body_children[action_index];
    if (!zox_valid(frame)) {
        zox_log_error("invalid frame_action")
        return;
    }
    entity frame_children[layouts2_children_capacity];
    uint frame_children_length = zox_get_children(world, frame, frame_children, layouts2_children_capacity);
    if (!frame_children_length ) {
        return;
    }
    entity icon = frame_children[0];
    if (!zox_valid(icon)) {
        zox_log_error("invalid icon_action")
        return;
    }
    // now reset icon
    set_icon_from_user_data(world, frame, icon, 0);
}*/

