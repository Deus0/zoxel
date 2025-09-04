entity spawn_item_pickedup(
    ecs *world,
    const entity prefab,
    const entity user,
    const byte quantity
) {
    const entity e = spawn_user_item(world, prefab, user);
    zox_set(e, Quantity, { quantity });
    zox_set(e, QuantityDirty, { zox_dirty_trigger });
    return e;
}