#include "overlay.c"
#include "tooltip.c"

void zox_define_systems_skills_ui(ecs *world) {
    zox_system(
        SkillOverlaySystem,
        EcsOnUpdate,
        [in] u.i.containers.DataLink,
        [none] elements2.Icon
    );
    zox_system(
        SkillIconTooltipSystem,
        EcsOnUpdate,
        [in] interaction.SelectState,
        [in] u.i.containers.DataLink,
        [none] elements2.Icon
    );
}
