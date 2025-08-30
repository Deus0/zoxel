entity spawn_user_item_quantity(
    ecs *world,
    const entity prefab,
    const entity user,
    const byte quantity
) {
    const entity e = spawn_user_item(world, prefab, user);
    zox_prefab_set(e, Quantity, { quantity });
    return e;
}
