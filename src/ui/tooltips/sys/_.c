#include "deselect.c"
#include "event.c"

void zox_define_systems_tooltips(ecs* world) {
    zox_system(
        TooltipDeselectSystem,
        EcsPreUpdate,
        [in] interaction.SelectState,
        [none] tooltips.Tooltipper
    );
    zox_system(
        TooltipEventSystem,
        EcsPostUpdate,
        [in] interaction.SelectState,
        [in] tooltips.TooltipEvent
    );
}
