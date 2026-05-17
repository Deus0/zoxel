byte max_stack_quantity = 255;

byte add_item_to_slot_manager(ecs* world, entity smanager, entity user, entity base_item, byte value) {
    if (!zox_valid(smanager)) {
        return 0;
    }
    entity slots[zox_children_capacity];
    uint slots_length = zox_get_slots_by_data(world, smanager, base_item, slots, zox_children_capacity);
    // zox_log("Item Stacking Slots [%s]:[%i]", zox_get_name(base_item), stacking_slots_length);
    entity stack_slot = 0;
    for (uint i = 0; i < slots_length; i++) {
        entity e2 = slots[i];
        if (!zox_valid(e2) || !zox_has(e2, DataLink)) {
            continue;
        }
        // zox_log("Stacking Slot [%i]:%s", i, zox_get_name(e2));
        entity e3 = zox_gett_value(e2, DataLink);
        if (!zox_valid(e3)) {
            continue;
        }
        // Check if MaxQuantity
        if (zox_gett_value(e3, Quantity) + value > max_stack_quantity) {
            continue;
        }
        stack_slot = e2;
        // zox_log("Stacking Slot Found [%i]:%s:%s", i, zox_get_name(e2), zox_get_name(e3));
        break;
    }
    if (stack_slot) {
        zox_set(stack_slot, DataDirty, { zox_dirty_trigger });
        entity e3 = zox_gett_value(stack_slot, DataLink);
        zox_geter_value(e3, Quantity, byte, quantity);
        zox_set(e3, Quantity, { quantity + value });
        zox_set(e3, QuantityDirty, { zox_dirty_trigger });
        return 1;
    }
    entity add_slot = zox_get_empty_slot(world, smanager);
    entity new_item = spawn_item_pickedup(world, base_item, user, value);
    zox_muter(add_slot, DataLink, slot_data);
    slot_data->value = new_item;
    zox_set(add_slot, DataDirty, { zox_dirty_trigger });
    return 1;
}

// TODO: Use System instead of Hook!
void on_overlap_pickup(ecs *world, entity e, entity user) {
    if (zox_gett_value(e, PickedUp) || !zox_has(user, PickUpperer)) {
        return;
    }
    // animate + picked up state
    zox_set(e, PickedUp, { pickup_state_trigger });
    zox_set(e, CollisionDisabled, { 1 });
    lerp_to_entity(world, e, user, 0.1f, 0.6f);
    zox_set(e, DestroyInTime, { 1 });
    if (!zox_has(e, ItemLink)) {
        return;
    }
    zox_geter_value(e, ItemLink, entity, base_item);
    if (!zox_valid(base_item)) {
        zox_loge("Pickup item is invalid");
        return;
    }
    // Try add to actionbar first
    entity actionbar = zox_get_child_by_id(world, user, zox_id(Actionbar));
    if (add_item_to_slot_manager(world, actionbar, user, base_item, 1)) {
        return;
    }
    entity inventory = zox_get_child_by_id(world, user, zox_id(Inventory));
    if (add_item_to_slot_manager(world, inventory, user, base_item, 1)) {
        return;
    }
}
