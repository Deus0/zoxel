entity spawn_item_pickedup(ecs *world, entity prefab, entity user, byte quantity) {
    entity e = spawn_user_item(world, user, prefab);
    zox_set(e, Quantity, { quantity });
    zox_set(e, QuantityDirty, { zox_dirty_trigger });
    return e;
}
