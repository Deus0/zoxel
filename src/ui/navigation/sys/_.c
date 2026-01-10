#include "begin.c"
#include "navigation.c"

void define_systems_navigation(ecs* world) {
    zox_system(
        ElementNavigationBeginSystem,
        EcsOnUpdate,
        [in] inputs.DeviceMode,
        [in] layouts2.CanvasLink,
        [out] raycasts.RaycasterTarget
    );
    zox_system(
        ElementNavigationSystem,
        EcsPostUpdate,
        [in] inputs.DeviceLinks,
        [in] inputs.DeviceMode,
        [out] elements.NavigatorState,
        [out] elements.NavigatorTimer,
        [out] raycasts.RaycasterTarget
    );
}