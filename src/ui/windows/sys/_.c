#include "layers.c"
#include "stack.c"
#include "clicked.c"
#include "cancel.c"
#include "constraints.c"

void zox_systems_windows(ecs* world) {
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
        // [in] SetWindowLayer,
        [in] windows.WindowLayer,
        [out] layouts.Layer,
        [none] windows.Window,
        [none] windows.WindowLayerDirty
    );
    zox_system(
        WindowElementClickedSystem,
        zoxp_update,
        [in] interactions.ClickState
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
        KeyboardCancelMenuSystem,
        zoxp_update,
        [in] inputs.DeviceDisabled,
        [in] inputs.Keyboard,
        [none] inputs.Device
    );
    zox_system(
        CanvasBoundsSystem,
        zoxp_initialize,
        [in] layouts.LayoutSize,
        [in] layouts.Anchor,
        [in] layouts.LayoutConstraints,
        [none] windows.Window,
        [none] core.Initialize,
    );
}
