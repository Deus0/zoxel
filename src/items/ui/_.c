/*
 *  Module: Items UI
 *
 *      - UIs for all our things
 *
 * */
#include "com/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "ins/_.c"
#include "eve/_.c"

void import_items_ui(ecs* world) {
    zox_module(items_ui);
    zox_components_items_ui(world);
    zox_systems_items_ui(world);
    add_hook_spawn_prefabs(zox_events_items_ui);
    add_taskbar_button(world, (TaskbarData) {
        .index = 3,
        .spawn = &spawn_menu_inventory,
        .component_id = zox_id(MenuItems),
        .texture_name = "taskbar_items",
        .tooltip_text = "Inventory"
    });

}
