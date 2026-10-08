#include "overlay.c"
#include "tooltip.c"

void zox_systems_skills_ui(ecs *world) {
    zox_system(
        SkillOverlaySystem,
        zoxp_update,
        [in] slots.DataLink,
        [none] elements2.Icon,
        // [none] skills_ui.SkillIcon,
    );
    zox_system(
        SkillIconTooltipSystem,
        zoxp_update,
        [in] interactions.SelectState,
        [in] slots.DataLink,
        [none] elements2.Icon
    );
}
