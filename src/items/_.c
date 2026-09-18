/*
 * +------------------------------------------------------------------+
 * | Zox Module: Items                                                |
 * |                                                                  |
 * |  Inventory - Consumables - Materials - Loot - Item State         |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
byte max_stack_quantity = 255;
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "ui/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "eve/_.c"
#include "tst/_.c"

void import_items(ecs* world) {
    zox_module(items);
    zox_components_items(world);
    zox_systems_items(world);
    zox_import_module(ItemsUI);
    add_hook_spawn_prefabs(zox_prefabs_items);
    add_hook_spawn_prefabs(zox_events_items);
}
