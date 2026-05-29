entity spawn_prefab_item_stackable(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("item_stackable");
    zox_prefab_set(e, Quantity, { 1 });
    zox_prefab_set(e, QuantityDirty, { zox_dirty_none });
    return e;
}
