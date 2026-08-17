/*
 * +------------------------------------------------------------------+
 * | Zox Module: Items                                                |
 * |                                                                  |
 * |  Inventory - Consumables - Materials - Loot - Item State         |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#if !defined(zoxm_items)
#define zoxm_items

byte max_stack_quantity = 255;
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "ui/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "tst/_.c"

zox_begin_module(Items) {
    define_components_items(world);
    define_systems_items(world);
    zox_import_module(ItemsUI);
    add_hook_spawn_prefabs(spawn_prefabs_items);
} zox_end_module(Items)

#endif
