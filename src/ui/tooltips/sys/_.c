#include "deselect.c"
#include "event.c"

void zox_define_systems_tooltips(ecs* world) {
    zox_system(
        TooltipDeselectSystem,
        zoxp_update, // EcsPreUpdate,
        [in] interaction.SelectState,
        [none] tooltips.Tooltipper
    );
    zox_system(
        TooltipEventSystem,
        zoxp_update, // EcsPostUpdate,
        [in] interaction.SelectState,
        [in] tooltips.TooltipEvent
    );
}
