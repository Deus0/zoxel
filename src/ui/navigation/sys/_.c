#include "begin.c"
#include "navigation.c"
#include "click.c"

void zox_systems_navigation(ecs* world) {
    zox_system(
        ElementNavigationBeginSystem,
        zoxp_update,
        [out] core.EntityTarget,
        [none] inputs.DeviceNavigator,
        [none] !core.Disabled,
    );
    zox_system(
        ElementNavigationSystem,
        zoxp_update,
        [out] ui.NavigatorState,
        [out] ui.NavigatorTimer,
        [out] core.EntityTarget,
        [none] inputs.DeviceNavigator,
        [none] !core.Disabled,
    );
    // NOTE: Has to be after raycasting system
    zox_system(
        DeviceClickSystem,
        zoxp_update,
        [in] core.EntityTarget,
        [out] interactions.ClickingEntity,
        [none] inputs.DeviceNavigator,
        [none] !core.Disabled,
    );
}
