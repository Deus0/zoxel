/*
 *  Module: Items UI
 *
 *      - UIs for all our things
 *
 * */
#ifndef zoxm_items_ui
#define zoxm_items_ui

#include "com/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "ins/_.c"

zox_begin_module(ItemsUI) {
    define_components_items_ui(world);
    define_systems_items_ui(world);
    add_taskbar_button((hook_taskbar) {
        .index = 3,
        .spawn = &spawn_menu_inventory,
        .component_id = MenuItems,
        .texture_name = "taskbar_items",
        .tooltip_text = "Inventory"
    });
} zox_end_module(ItemsUI)

#endif
