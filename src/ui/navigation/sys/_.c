#include "begin.c"
#include "navigation.c"

void define_systems_navigation(ecs* world) {
    zox_system(
        ElementNavigationBeginSystem,
        zoxp_update,
        [in] inputs.DeviceMode,
        [in] layouts.CanvasLink,
        [out] raycasts.RaycasterTarget
    );
    zox_system(
        ElementNavigationSystem,
        zoxp_update, // EcsPostUpdate,
        [in] inputs.DeviceMode,
        [out] elements.NavigatorState,
        [out] elements.NavigatorTimer,
        [out] raycasts.RaycasterTarget
    );
}
