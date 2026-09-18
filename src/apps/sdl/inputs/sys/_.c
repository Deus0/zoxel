#include "mouse.c"
#include "mouse_constrain.c"
#include "keyboard.c"
#include "touchscreen.c"
#include "controller.c"

void zox_systems_sdl_inputs(ecs* world) {
    zox_system_1(
        mouse_input_system,
        zoxp_inputs_extract,
        [none] inputs.Mouse
    );
    // NOTE: We must constrain after extracting
    zox_system_1(
        MouseConstrainSystem,
        zoxp_inputs_extract,
        [in] inputs.DeviceDisabled,
        [in] inputs.MouseLock,
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
        [out] core.PixelSize,
        [none] inputs.Touchscreen
    );
    zox_system_1(
        gamepad_extract_system,
        zoxp_inputs_extract,
        [out] SdlGameController,
        [out] inputs.DeviceHasInput,
        [none] inputs.Gamepad
    );
}
