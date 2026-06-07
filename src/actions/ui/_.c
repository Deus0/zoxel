#ifndef zoxm_actions_ui
#define zoxm_actions_ui

#include "com/_.c"
#include "set/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"

zox_begin_module(ActionsUI) {
    zox_define_components_actions_ui(world);
    // add_hook_spawn_prefabs(spawn_prefabs_ui_actions);
    add_taskbar_button((hook_taskbar) {
        .index = 1,
        .spawn = &spawn_menu_actions,
        .component_id = MenuActions,
        .texture_name = "taskbar_actions",
        .tooltip_text = "Actionbar"
    });
} zox_end_module(Actions);

#endif
