#include "begin.c"
#include "navigation.c"
#include "click.c"

void define_systems_navigation(ecs* world) {
    zox_system(
        ElementNavigationBeginSystem,
        zoxp_update,
        [in] inputs.DeviceDisabled,
        [out] raycasts.RaycasterTarget,
        [none] inputs.DeviceNavigator
    );
    zox_system(
        ElementNavigationSystem,
        zoxp_update,
        // [in] inputs.DeviceMode,
        [in] inputs.DeviceDisabled,
        [out] elements.NavigatorState,
        [out] elements.NavigatorTimer,
        [out] raycasts.RaycasterTarget,
        [none] inputs.DeviceNavigator
    );
    // NOTE: Has to be after raycasting system
    zox_system(
        DeviceClickSystem,
        zoxp_update,
        [in] inputs.DeviceDisabled,
        [in] raycasts.RaycasterTarget,
        [out] interaction.ClickingEntity,
        [none] inputs.DeviceNavigator
    );
}
