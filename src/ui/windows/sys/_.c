#include "layers.c"
#include "stack.c"
#include "clicked.c"
#include "cancel.c"
#include "constraints.c"
#include "layout.c"
#include "search.c"

void zox_systems_windows(ecs* world) {
    zox_system(
        canvas_stack_system,
        zoxp_update,
        [out] layouts.WindowToTop,
        [out] WindowsLayers,
        [out] WindowsCount,
        [none] layouts.Canvas
    );
    zox_system(
        window_layer_system,
        zoxp_update,
        // [in] SetWindowLayer,
        [in] windows.WindowLayer,
        [out] layouts.Layer,
        [none] windows.Window,
        [none] windows.WindowLayerDirty
    );
    zox_on_add(
        window_element_clicked_event,
        [none] ui.Element,
        [none] interactions.Press,    // to the top on press, not waiting for the click
    );
    // TODO: Add keyboard escape to this
    zox_system(
        cancel_menu_system,
        zoxp_update,
        [in] inputs.DeviceButtonType,
        [in] inputs.ZeviceButton,
        [none] inputs.Zevice,
        [none] !core.Disabled,
    );
    zox_system(
        keyboard_cancel_menu_system,
        zoxp_update,
        [in] inputs.Keyboard,
        [none] inputs.Device,
        [none] !core.Disabled,
    );
    zox_system(
        canvas_bounds_system,
        zoxp_initialize,
        [in] layouts.LayoutSize,
        [in] layouts.Anchor,
        [in] layouts.LayoutConstraints,
        [none] windows.Window,
        [none] !docks.Docked,
        [none] core.Initialize,
    );
}
