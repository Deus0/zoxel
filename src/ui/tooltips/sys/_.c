#include "deselect.c"
#include "event.c"
#include "link.c"
#include "unlink.c"

void zox_systems_tooltips(ecs* world) {
    zox_system(
        tooltip_deselect_system,
        zoxp_update,
        [out] texts.TextData,
        [none] texts.Text,
        [none] tooltips.Tooltip,
    );
    // We can use later to clean hierarchys
    zox_system(
        tooltipee_initialize_system,
        zoxp_initialize,
        [none] tooltips.Tooltipper,
        [none] core.Initialize,
    );
    zox_system(
        tooltipee_deselect_system,
        zoxp_update,
        [none] ui.Element,
        // [none] interactions.Selectable,
        [none] tooltips.Tooltipper,
        [none] interactions.Deselect,
        //[in] interactions.SelectState,
    );
    zox_system(
        tooltip_event_system,
        zoxp_update,
        // [in] interactions.SelectState,
        [in] tooltips.TooltipEvent,
        [none] tooltips.Tooltipper,
        [none] interactions.Select,
    );
}
