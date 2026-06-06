#include "layers.c"
#include "stack.c"
#include "clicked.c"
#include "cancel.c"
#include "constraints.c"

void define_systems_windows(ecs* world) {
    zox_system(
        CanvasStackSystem,
        EcsOnLoad,
        [out] layouts2.WindowToTop,
        [out] WindowsLayers,
        [out] WindowsCount,
        [none] layouts2.Canvas
    );
    zox_system(
        WindowLayerSystem,
        EcsOnLoad,
        [in] SetWindowLayer,
        [out] WindowLayer,
        [out] layouts2.Layer2D,
        [none] Window
    );
    zox_system(
        WindowElementClickedSystem,
        EcsOnUpdate,
        [in] interaction.ClickState
    );
    // TODO: Add keyboard escape to this
    zox_system(
        CancelMenuSystem,
        EcsPostUpdate,
        [in] inputs.ZeviceDisabled,
        [in] inputs.DeviceButtonType,
        [in] inputs.ZeviceButton,
        [none] inputs.Zevice
    );
    zox_system(
        CanvasBoundsSystem,
        EcsOnUpdate,
        [in] core.InitializeEntity,
        [in] layouts2.LayoutSize,
        [in] layouts2.Anchor,
        [in] layouts2.LayoutConstraints,
        [none] windows.Window
    );
}
