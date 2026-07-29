#include "layers.c"
#include "stack.c"
#include "clicked.c"
#include "cancel.c"
#include "constraints.c"

void define_systems_windows(ecs* world) {
    zox_system(
        CanvasStackSystem,
        zoxp_update,
        [out] layouts.WindowToTop,
        [out] WindowsLayers,
        [out] WindowsCount,
        [none] layouts.Canvas
    );
    zox_system(
        WindowLayerSystem,
        zoxp_update,
        [in] SetWindowLayer,
        [out] WindowLayer,
        [out] layouts.Layer2D,
        [none] Window
    );
    zox_system(
        WindowElementClickedSystem,
        zoxp_update,
        [in] interaction.ClickState
    );
    // TODO: Add keyboard escape to this
    zox_system(
        CancelMenuSystem,
        zoxp_update,
        [in] inputs.ZeviceDisabled,
        [in] inputs.DeviceButtonType,
        [in] inputs.ZeviceButton,
        [none] inputs.Zevice
    );
    zox_system(
        CanvasBoundsSystem,
        zoxp_update,
        [in] core.Initialize,
        [in] layouts.LayoutSize,
        [in] layouts.Anchor,
        [in] layouts.LayoutConstraints,
        [none] windows.Window
    );
}
