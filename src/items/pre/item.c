entity spawn_prefab_item(ecs* world) {
    zox_prefab();
    zox_prefab_name("item");
    zox_add_tag(e, Item);
    zox_prefab_set(e, ZoxName, { 0 });
    zox_prefab_set(e, TextureLink, { 0 });
    zox_prefab_set(e, Quantity, { 1 });
    zox_prefab_set(e, ItemDirty, { zox_dirty_none });
    zox_prefab_set(e, QuantityDirty, { zox_dirty_none });
    return e;
}
