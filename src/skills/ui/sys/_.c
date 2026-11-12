#include "overlay.c"
#include "tooltip.c"

void zox_systems_skills_ui(ecs *world) {
    zox_system(
        skill_overlay_system,
        zoxp_update,
        [none] elements2.Icon,
        [none] ui_containers.DataUI,
        // [none] skills_ui.SkillIcon,
    );
    zox_system(
        skill_icon_tooltip_system,
        zoxp_update,
        [in] interactions.SelectState,
        [none] elements2.Icon,
        [none] ui_containers.DataUI,
    );
}
