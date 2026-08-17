/*
 * +------------------------------------------------------------------+
 * | Zox Module: Crafting                                             |
 * |                                                                  |
 * |  Recipes - Materials - Stations - Crafting - Production          |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#if !defined(zoxm_crafting) && defined(zoxm_items)
#define zoxm_crafting

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Crafting) {
    define_components_crafting(world);
    define_systems_crafting(world);
    add_hook_spawn_prefabs(spawn_prefabs_crafting);
    add_taskbar_button((hook_taskbar) {
        .index = 9,
        .spawn = &spawn_player_menu_crafting,
        .component_id = MenuCrafting,
        .texture_name = "taskbar_crafting",
        .tooltip_text = "Craftbench"
    });
} zox_end_module(Crafting);

#endif
