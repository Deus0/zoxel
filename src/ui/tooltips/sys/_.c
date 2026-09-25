#include "deselect.c"
#include "event.c"
#include "link.c"
#include "unlink.c"

void zox_systems_tooltips(ecs* world) {
    zox_system(
        TooltipDeselectSystem,
        zoxp_update,
        [out] texts.TextData,
        [none] texts.Text,
        [none] tooltips.Tooltip,
    );
    // We can use later to clean hierarchys
    zox_system(
        TooltipeeInitializeSystem,
        zoxp_initialize,
        [none] tooltips.Tooltipper,
        [none] core.Initialize,
    );
    zox_system(
        TooltipeeDeselectSystem,
        zoxp_update,
        [none] ui.Element,
        // [none] interactions.Selectable,
        [none] tooltips.Tooltipper,
        [none] interactions.Deselect,
        //[in] interactions.SelectState,
    );
    zox_system(
        TooltipEventSystem,
        zoxp_update,
        [in] interactions.SelectState,
        [in] tooltips.TooltipEvent,
        [none] tooltips.Tooltipper
    );
}
