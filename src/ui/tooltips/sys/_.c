#include "deselect.c"
#include "event.c"
#include "link.c"
#include "unlink.c"
#include "follow3.c"
#include "navigated.c"

void zox_systems_tooltips(ecs* world) {
    // after mouse_element_system (zoxp_update: it'd put it back at the mouse), before the layouts: navigating, under
    //  the selected ui instead
    zox_system(
        tooltip_navigated_system,
        zoxp_post_update,
        [in] layouts.Anchor,
        [out] layouts.LayoutPosition,
        [none] tooltips.Tooltip,
        [none] ui.MouseElement,
    );
    // multi threaded: it writes only its own transform (reads its ui's and window's)
    zox_system(
        tooltip3_follow_system,
        zoxp_update,
        [in] layouts.LayoutSize,
        [out] transforms3.Position3D,
        [out] transforms3.Rotation3D,
        [none] tooltips.Tooltip,
        [none] transforms.DisableParentTransform,
    );
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
