#include "mouse.c"
#include "mouse_constrain.c"
#include "keyboard.c"
#include "touchscreen.c"
#include "controller.c"

void zox_define_systems_sdl_inputs(ecs* world) {
    zox_system_1(
        MouseExtractSystem,
        zoxp_inputs_extract,
        [in] apps.AppLink,
        [none] inputs.Mouse
    );
    // NOTE: We must constrain after extracting
    zox_system_1(
        MouseConstrainSystem,
        zoxp_inputs_extract,
        [in] inputs.DeviceDisabled,
        [in] inputs.MouseLock,
        [in] apps.AppLink,
        [none] inputs.Mouse
    );
    zox_system_1(
        KeyboardExtractSystem,
        zoxp_inputs_extract,
        [out] inputs.Keyboard,
       // [none] inputs.Keyboard
    );
    zox_system_1(
        TouchscreenExtractSystem,
        zoxp_inputs_extract,
        [in] apps.AppLink,
        [out] core.PixelSize,
        [none] inputs.Touchscreen
    );
    zox_system_1(
        SdlControllerFetchSystem,
        zoxp_inputs_extract,
        [in] SdlGameController,
        [none] inputs.Gamepad
    );
}
