#include "touchscreen.c"
#include "mouse.c"
#include "mouse_constrain.c"
// #include "joystick.c"
#include "controller.c"

void zox_define_systems_sdl_inputs(ecs* world) {
    zox_system_1(
        MouseExtractSystem,
        zoxp_sdl,
        [in] apps.AppLink,
        [none] inputs.Mouse
    );
    zox_system_1(
        MouseConstrainSystem,
        zoxp_sdl,
        [in] inputs.DeviceDisabled,
        [in] inputs.MouseLock,
        [in] apps.AppLink,
        [none] inputs.Mouse
    );
    zox_system_1(
        TouchscreenExtractSystem,
        zoxp_sdl,
        [in] apps.AppLink,
        [out] screens.ScreenDimensions,
        [none] inputs.Touchscreen
    );
    zox_system_1(
        SdlControllerFetchSystem,
        zoxp_sdl,
        [in] SdlGameController,
        [none] inputs.Gamepad
    );
}
