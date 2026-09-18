#ifndef zoxm_skills_ui
#define zoxm_skills_ui

#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(UISkills) {
    zox_components_skills_ui(world);
    zox_systems_skills_ui(world);
    // add_hook_spawn_prefabs(spawn_prefabs_ui_skills);
    add_taskbar_button(world, (TaskbarData) {
        .index = 4,
        .spawn = &spawn_player_menu_skills,
        .component_id = zox_id(MenuSkills),
        .texture_name = "taskbar_skills",
        .tooltip_text = "Skillbook"
    });
} zox_end_module(UISkills)

#endif
