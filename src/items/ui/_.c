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
    zox_components_items_ui(world);
    zox_systems_items_ui(world);
    add_taskbar_button(world, (TaskbarData) {
        .index = 3,
        .spawn = &spawn_menu_inventory,
        .component_id = zox_id(MenuItems),
        .texture_name = "taskbar_items",
        .tooltip_text = "Inventory"
    });
} zox_end_module(ItemsUI)

#endif
