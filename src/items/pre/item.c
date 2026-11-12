entity spawn_prefab_item(ecs* world) {
    zox_prefab();
    zox_prefab_name("item");
    zox_add(e, Item);
    zox_set(e, ZoxName, { 0 });
    // zox_set(e, TextureLink, { 0 });
    zox_set(e, ItemDirty, { zox_dirty_none });
    return e;
}
