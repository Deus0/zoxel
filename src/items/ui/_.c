#ifndef zoxm_items_ui
#define zoxm_items_ui

zox_tag(MenuItems);
zox_tag(IconItem);
zox_tag(ItemIconLabel);
#include "fun/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "ins/_.c"

zox_begin_module(ItemsUI)
    zoxd_tag(MenuItems);
    zoxd_tag(IconItem);
    zoxd_tag(ItemIconLabel);
    add_taskbar_button((hook_taskbar) {
        .index = 3,
        .spawn = &spawn_player_menu_items,
        .component_id = MenuItems,
        .texture_name = "taskbar_items",
        .tooltip_text = "Inventory"
    });
    add_hook_spawn_prefabs(spawn_prefabs_ui_items);
    define_systems_items_ui(world);
zox_end_module(ItemsUI)

#endif
