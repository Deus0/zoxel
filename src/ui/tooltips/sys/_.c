#include "deselect.c"
#include "event.c"

void zox_define_systems_tooltips(ecs* world) {
    zox_system(
        TooltipDeselectSystem,
        EcsPreUpdate,
        [in] elements.SelectState,
        [none] tooltips.Tooltipper
    );
    zox_system(
        TooltipEventSystem,
        EcsPostUpdate,
        [in] elements.SelectState,
        [in] tooltips.TooltipEvent
    );
}
