#include "overlay.c"
#include "tooltip.c"

void zox_define_systems_skills_ui(ecs *world) {
    zox_system(
        SkillOverlaySystem,
        EcsOnUpdate,
        [in] slots.DataLink,
        [none] elements2.Icon
    );
    zox_system(
        SkillIconTooltipSystem,
        EcsOnUpdate,
        [in] interaction.SelectState,
        [in] slots.DataLink,
        [none] elements2.Icon
    );
}
