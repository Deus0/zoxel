#include "item.c"
#include "stackable.c"
#include "active.c"
entity prefab_item;
entity prefab_item_stackable;
entity prefab_item_active;

void spawn_prefabs_items(ecs *world) {
    prefab_item = spawn_prefab_item(world);
    prefab_item_stackable = spawn_prefab_item_stackable(world, prefab_item);
    prefab_item_active = spawn_prefab_item_active(world, prefab_item_stackable);
    if (prefab_realm) {
        zox_prefab_add(prefab_realm, ItemLinks);
    }
}
