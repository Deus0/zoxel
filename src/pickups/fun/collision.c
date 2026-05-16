const byte max_stack_quantity = 255;

// TODO: system state instead of events
void on_overlap_pickup(ecs *world, entity e, entity user) {
    if (zox_gett_value(e, PickedUp) || !zox_has(user, PickUpperer)) {
        return;
    }
    // animate + picked up state
    zox_set(e, PickedUp, { pickup_state_trigger });
    zox_set(e, CollisionDisabled, { 1 });
    lerp_to_entity(world, e, user, 0.1f, 0.6f);
    zox_set(e, DestroyInTime, { 1 });
    // zox_log(" > e [%lu] picked up by user [%lu]\n", e, user)
    if (!zox_has(e, ItemLink)) {
        return;
    }
    zox_geter_value(e, ItemLink, entity, item);
    if (!zox_valid(item)) {
        zox_loge("Pickup item is invalid");
        return;
    }
    entity actionbar = zox_get_child_by_id(world, user, zox_id(Actionbar));
    if (!zox_valid(actionbar)) {
        zox_loge("Cannot Pickup without Actionbar Slots");
        return;
    }
    entity inventory = zox_get_child_by_id(world, user, zox_id(Inventory));
    if (!zox_valid(inventory)) {
        zox_loge("Cannot Pickup without Inventory Slots");
        return;
    }
    entity slot = 0;
    entity action_slot = zox_get_empty_slot(world, actionbar);
    entity item_slot = zox_get_empty_slot(world, inventory);
    if (!slot) {
        slot = action_slot;
    }
    if (!slot) {
        slot = item_slot;
    }
    if (!zox_valid(slot)) {
        zox_loge("No Empty Slot for new Item");
        return;
    }
    entity e2 = spawn_item_pickedup(world, item, user, 1);
    zox_muter(slot, DataLink, slot_data);
    slot_data->value = e2;
    zox_set(slot, DataDirty, { zox_dirty_trigger });
    // TODO: We need to set UI to dirty too
    // TODO: Get Stack Index, Check all slots

    /*byte stack_index = 255;
    for (int i = 0; i < actions->length; i++) {
        entity action = actions->value[i];
        if (!zox_valid(action)) {
            continue;
        }
        zox_get_prefab(action, item_prefab);
        if (item_prefab == item) {
            stack_index = i;
            break;
        }
    }*/
    // stack first
    /*byte did_stack = 0;
    if (stack_index != 255) {
        entity stack_item = actions->value[stack_index];
        zox_geter_value(stack_item, Quantity, byte, quantity);
        if (quantity != max_stack_quantity) {
            quantity++;
            zox_set(stack_item, Quantity, { quantity });
            zox_set(stack_item, QuantityDirty, { zox_dirty_trigger });
            did_stack = 1;
        }
    }
    if (!did_stack) {
        // place as new
        byte action_index = 255;
        for (int i = 0; i < actions->length; i++) {
            if (actions->value[i] == 0) {
                action_index = i;
                break;
            }
        }
        // zox_get_prefab(item, item)
        if (action_index == 255 || !item) {
            // zox_log(" ! cannot  pickup, full or item is [%lu]\n", item)
            return;
        }
        // actions->value[action_index] = new_item;
        // on_action_set(world, user, action_index, new_item, item);
    }*/
}
