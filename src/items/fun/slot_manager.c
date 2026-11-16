// puts an item in a slot manager (an inventory, an actionbar): stacked onto one holding it, else a new one in an
//  empty slot, 1 if it went in (picking things up, quest rewards); it was in pickups, quests reached up for it
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
            if (!zox_valid(e2)) {
                continue;
            }
            // zox_log("Stacking Slot [%i]:%s", i, zox_get_name(e2));
            entity e3 = zox_get_link(world, e2, DataLink);
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
        entity item = zox_get_link(world, stack_slot, DataLink);
        byte quantity = zox_getv(item, Quantity);
        zox_setv(item, Quantity, quantity + value);
        zox_add(item, QuantityDirty);
        zox_add(stack_slot, DataDirty);
        return 1;
    }
    entity add_slot = zox_get_empty_slot(world, smanager);
    if (zox_valid(add_slot)) {
        entity new_item = spawn_item_pickedup(
            world,
            base_item,
            user,
            value);
        zox_set_link(world, add_slot, DataLink, new_item);
        zox_add(add_slot, DataDirty);
        return 1;
    }
    return 0;
}
