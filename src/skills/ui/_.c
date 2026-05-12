#ifndef zoxm_skills_ui
#define zoxm_skills_ui

zox_tag(MenuSkills);
zox_tag(IconSkill);
zox_tag(FrameSkill);
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/skill_overlay.c"

zox_begin_module(UISkills)
    zoxd_tag(MenuSkills);
    zoxd_tag(IconSkill);
    zoxd_tag(FrameSkill);
    // note: action uis doesn't have same things'
    zox_system(
        SkillOverlaySystem,
        EcsOnUpdate,
        [in] u.i.containers.DataLink,
        [none] elements2.Icon
    );
    add_hook_spawn_prefabs(spawn_prefabs_ui_skills);
    add_taskbar_button((hook_taskbar) {
        .index = 4,
        .spawn = &spawn_player_menu_skills,
        .component_id = MenuSkills,
        .texture_name = "taskbar_skills",
        .tooltip_text = "Skillbook"
    });
zox_end_module(UISkills)

#endif
