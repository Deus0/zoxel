#include "begin.c"
#include "navigation.c"
#include "click.c"

void zox_systems_navigation(ecs* world) {
    zox_system(
        element_navigation_begin_system,
        zoxp_update,
        [out] core.EntityTarget,
        [none] inputs.DeviceNavigator,
        [none] !core.Disabled,
    );
    zox_system(
        element_navigation_system,
        zoxp_update,
        [out] ui.NavigatorState,
        [out] ui.NavigatorTimer,
        [out] core.EntityTarget,
        [none] inputs.DeviceNavigator,
        [none] !core.Disabled,
    );
    // NOTE: Has to be after raycasting system
    zox_system(
        device_click_system,
        zoxp_update,
        [in] core.EntityTarget,
        [out] interactions.ClickingEntity,
        [none] inputs.DeviceNavigator,
        [none] !core.Disabled,
    );
}
