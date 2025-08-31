zoxf_user_prefabs(Item, item, "item")
#include "user_item.c"

void spawn_prefabs_items(ecs *world) {
    prefab_item = spawn_prefab_item(world);
    zox_set(prefab_item, Quantity, { 1 });
    zox_set(prefab_item, ItemDirty, { zox_dirty_none });
    zox_set(prefab_item, QuantityDirty, { zox_dirty_none });
    zox_prefab_character_add(ItemLinks);
    if (prefab_realm) {
        zox_prefab_add(prefab_realm, ItemLinks);
    }
}