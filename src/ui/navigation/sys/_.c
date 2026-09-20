#include "begin.c"
#include "navigation.c"
#include "click.c"

void zox_systems_navigation(ecs* world) {
    zox_system(
        ElementNavigationBeginSystem,
        zoxp_update,
        [in] inputs.DeviceDisabled,
        [out] core.EntityTarget,
        [none] inputs.DeviceNavigator
    );
    zox_system(
        ElementNavigationSystem,
        zoxp_update,
        // [in] inputs.DeviceMode,
        [in] inputs.DeviceDisabled,
        [out] ui.NavigatorState,
        [out] ui.NavigatorTimer,
        [out] core.EntityTarget,
        [none] inputs.DeviceNavigator
    );
    // NOTE: Has to be after raycasting system
    zox_system(
        DeviceClickSystem,
        zoxp_update,
        [in] inputs.DeviceDisabled,
        [in] core.EntityTarget,
        [out] interactions.ClickingEntity,
        [none] inputs.DeviceNavigator
    );
}
