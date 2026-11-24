#include "begin.c"
#include "navigation.c"
#include "click.c"
#include "gamepad_ui.c"

void zox_systems_navigation(ecs* world) {
    // main thread: slide and tab events set settings and texts
    zox_system_1(
        navigation_slider_system,
        zoxp_spawn,
        [in] core.Target,
        [none] inputs.DeviceNavigator,
        [none] !core.Disabled
    );
    zox_system_1(
        navigation_tabs_system,
        zoxp_spawn,
        [in] core.Target,
        [none] inputs.DeviceNavigator,
        [none] !core.Disabled
    );
    zox_system(
        element_navigation_begin_system,
        zoxp_update,
        [out] core.Target,
        [none] inputs.DeviceNavigator,
        [none] !core.Disabled,
    );
    zox_system(
        element_navigation_system,
        zoxp_update,
        [out] ui.NavigatorState,
        [out] ui.NavigatorTimer,
        [out] core.Target,
        [none] inputs.DeviceNavigator,
        [none] !core.Disabled,
    );
    // NOTE: Has to be after raycasting system
    zox_system(
        device_click_system,
        zoxp_update,
        [in] core.Target,
        [out] interactions.ClickingEntity,
        [none] inputs.DeviceNavigator,
        [none] !core.Disabled,
    );
}
