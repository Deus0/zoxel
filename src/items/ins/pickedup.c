entity spawn_item_pickedup(
    ecs *world,
    entity prefab,
    entity user,
    byte quantity)
{
    entity e = spawn_user_item(world, user, prefab);
    zox_setv(e, Quantity, quantity);
    zox_add(e, QuantityDirty);
    return e;
}
