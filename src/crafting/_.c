/*
 * +------------------------------------------------------------------+
 * | Zox Module: Crafting                                             |
 * |                                                                  |
 * |  Recipes - Materials - Stations - Crafting - Production          |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

void import_crafting(ecs* world) {
    zox_module(crafting);
    define_components_crafting(world);
    define_systems_crafting(world);
    add_hook_spawn_prefabs(spawn_prefabs_crafting);
    add_taskbar_button(world, (TaskbarData) {
        .index = 9,
        .spawn = &spawn_player_menu_crafting,
        .component_id = zox_id(MenuCrafting),
        .texture_name = "taskbar_crafting",
        .tooltip_text = "Craftbench"
    });
}
