#ifndef zoxm_quests_ui
#define zoxm_quests_ui

zox_tag(MenuQuests);
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(QuestsUI) {
    zoxd_tag(MenuQuests);
    zox_define_systems_quests_ui(world);
    // add_hook_spawn_prefabs(spawn_prefabs_ui_quests);
    add_taskbar_button((hook_taskbar) {
        .index = 5,
        .spawn = &spawn_player_menu_quests,
        .component_id = MenuQuests,
        .texture_name = "taskbar_quests",
        .tooltip_text = "Questlog"
    });
} zox_end_module(UIQuests)

#endif
