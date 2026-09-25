byte add_item_to_slot_manager(
    ecs* world,
    entity smanager,
    entity user,
    entity base_item,
    byte value)
{
    if (!zox_valid(smanager) ||
        !zox_valid(user) ||
        !zox_valid(base_item) ||
        !value)
    {
        return 0;
    }
    entity slots[zox_children_capacity];
    uint slots_length = zox_get_slots_by_data(
        world,
        smanager,
        base_item,
        slots,
        zox_children_capacity);
    // zox_log("Item Stacking Slots [%s]:[%i]", zox_get_name(base_item), stacking_slots_length);
    byte can_stack = zox_has(base_item, Quantity);
    entity stack_slot = 0;
    if (can_stack) {
        for (uint i = 0; i < slots_length; i++) {
            entity e2 = slots[i];
            if (!zox_valid(e2) || !zox_has(e2, DataLink)) {
                continue;
            }
            // zox_log("Stacking Slot [%i]:%s", i, zox_get_name(e2));
            entity e3 = zox_getv(e2, DataLink);
            if (!zox_valid(e3) || !zox_has(e3, Quantity)) {
                continue;
            }
            // Check if MaxQuantity
            if (zox_getv(e3, Quantity) + value > max_stack_quantity) {
                continue;
            }
            stack_slot = e2;
            // zox_log("Stacking Slot Found [%i]:%s:%s", i, zox_get_name(e2), zox_get_name(e3));
            break;
        }
    } else {
        if (value > 1) {
            zox_loge("Trying to stack a item [%s] with no quantity!",
                zox_getn(base_item));
            value = 1;
        }
    }
    if (stack_slot) {
        entity item = zox_getv(stack_slot, DataLink);
        byte quantity = zox_getv(item, Quantity);
        zox_setv(item, Quantity, quantity + value);
        zox_add(item, QuantityDirty);
        zox_add(stack_slot, DataDirty);
        return 1;
    }
    entity add_slot = zox_get_empty_slot(world, smanager);
    if (zox_valid(add_slot)) {
        zox_muter(add_slot, DataLink, slot_data);
        entity new_item = spawn_item_pickedup(
            world,
            base_item,
            user,
            value);
        slot_data->value = new_item;
        zox_add(add_slot, DataDirty);
        return 1;
    }
    return 0;
}

void pickup_ending(
    ecs* world,
    entity user,
    entity pickup)
{
    // If bugged
    entity base_item = zox_get_link(world, pickup, ItemLink);
    if (!zox_valid(base_item)) {
        zox_loge("Pickup item is invalid");
        zox_delete(pickup);
        return;
    }
    byte quantity = zox_has(pickup, Quantity) ?
        zox_getv(pickup, Quantity) : 1;
    // Try add to actionbar first
    entity actionbar = zox_get_child_by_id(world, user, zox_id(Actionbar));
    if (add_item_to_slot_manager(
        world,
        actionbar,
        user,
        base_item,
        quantity))
    {
        zox_delete(pickup);
        return;
    }
    entity inventory = zox_get_child_by_id(world, user, zox_id(Inventory));
    if (add_item_to_slot_manager(
        world,
        inventory,
        user,
        base_item,
        quantity))
    {
        zox_delete(pickup);
        return;
    }
    // reset
    zox_logw("Item failed to add to user [%s]",
        zox_getn(base_item));
    zox_setv(pickup, PickedUp, 0);
    zox_setv(pickup, CollisionDisabled, 0);
}

// TODO: Use System instead of Hook!
byte on_overlap_pickup(
    ecs *world,
    entity pickup,
    entity user)
{
    if (!zox_valid(pickup) ||
        !zox_valid(user))
    {
        return 0;
    }
    if (zox_getv(pickup, PickedUp) ||
        !zox_has(user, PickUpperer))
    {
        return 0;
    }
    if (zox_has(user, Dead)) {
        zox_setv(pickup, PickedUp, pickup_state_none);
        return 0;
    }
    // animate + picked up state
    zox_setv(pickup, PickedUp, pickup_state_trigger);
    zox_setv(pickup, CollisionDisabled, 1);
    lerp_to_entity(world, pickup, user, 0.1f, 0.6f);
    delay_event2(
        world,
        pickup_ending,
        user,
        pickup,
        0.6f);
    return 0;
}
