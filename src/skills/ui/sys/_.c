#include "overlay.c"
#include "tooltip.c"

void zox_systems_skills_ui(ecs *world) {
    zox_system(
        SkillOverlaySystem,
        zoxp_update,
        [none] elements2.Icon,
        [none] ui_containers.DataUI,
        // [none] skills_ui.SkillIcon,
    );
    zox_system(
        SkillIconTooltipSystem,
        zoxp_update,
        [in] interactions.SelectState,
        [none] elements2.Icon,
        [none] ui_containers.DataUI,
    );
}
