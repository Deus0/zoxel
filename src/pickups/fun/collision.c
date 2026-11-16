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
