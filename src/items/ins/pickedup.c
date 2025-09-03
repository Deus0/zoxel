entity spawn_item_pickedup(
    ecs *world,
    const entity prefab,
    const entity user,
    const byte quantity
) {
    const entity e = spawn_user_item(world, prefab, user);
    zox_set(e, Quantity, { quantity });
    zox_set(e, QuantityDirty, { zox_dirty_trigger });
    // zox_prefab_addc_user_timings(world, e);
    /*zox_log("picked up item: %s %i",
        zox_get_name(prefab),
        zox_has(prefab, WarmupTime));*/
    return e;
}