#include "overlay.c"
#include "tooltip.c"

void zox_define_systems_skills_ui(ecs *world) {
    zox_system(
        SkillOverlaySystem,
        zoxp_update,
        [in] slots.DataLink,
        [none] elements2.Icon
    );
    zox_system(
        SkillIconTooltipSystem,
        zoxp_update,
        [in] interaction.SelectState,
        [in] slots.DataLink,
        [none] elements2.Icon
    );
}
