#include "layers.c"
#include "stack.c"
#include "clicked.c"
#include "cancel.c"

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
        [in] layouts2.CanvasLink,
        [out] WindowLayer,
        [out] layouts2.Layer2D,
        [none] Window
    );
    zox_system(
        WindowElementClickedSystem,
        EcsOnUpdate,
        [in] interaction.ClickState
    );
    zox_system(
        CancelMenuSystem,
        EcsPostUpdate,
        [in] inputs.ZeviceDisabled,
        [in] inputs.DeviceButtonType,
        [in] inputs.ZeviceButton,
        [none] inputs.Zevice
    );
}
