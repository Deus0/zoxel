entity spawn_item_pickedup(ecs *world, entity p, entity user, byte quantity) {
    entity e = spawn_user_item(world, p, user);

    zox_set(e, Quantity, { quantity });
    zox_set(e, QuantityDirty, { zox_dirty_trigger });

    return e;
}
