#include "reenable.c"
#include "reset.c"
#include "device_mode.c"

void zox_systems_input(ecs* world) {
    // resets
    zox_system(
        zevice_pointer_reset_system,
        zoxp_inputs_reset,
        [out] inputs.ZevicePointer,
        [out] inputs.ZevicePointerOld,
        [none] !inputs.Finger
    );
    zox_system(
        zevice_button_reset_system,
        zoxp_inputs_reset,
        [out] inputs.ZeviceButton,
        [none] !inputs.Finger
    );
    zox_system(
        zevice_button_enable_system,
        zoxp_inputs_enable,
        [in] inputs.ZeviceButton,
        [none] !inputs.Finger,
        [none] core.Disabled,
    );
    zox_system(
        zevice_stick_enable_system,
        zoxp_inputs_enable,
        [in] inputs.ZeviceStick,
        [none] !inputs.Finger,
        [none] core.Disabled,
    );
    zox_system(
        zevice_pointer_enable_system,
        zoxp_inputs_enable,
        [in] inputs.ZevicePointer,
        [none] !inputs.Finger,
        [none] core.Disabled,
    );
    /*zox_system(
        zevice_finger_reset_system,
        zoxp_inputs_reset,
        [in] ZevicePointerOld,
        [out] ZevicePointerPosition,
        [out] ZevicePointerDelta,
        [none] Finger
    );*/
    zox_system(
        zevice_pointer_right_reset_system,
        zoxp_inputs_reset,
        [out] inputs.ZevicePointerRight
    );
    zox_system(
        zevice_pointer_delta_reset_system,
        zoxp_inputs_reset,
        [out] inputs.ZevicePointerDelta
    );
    zox_system(
        keyboard_reset_system,
        zoxp_inputs_reset,
        [out] inputs.Keyboard
    );
    // state changes
    zox_system(
        device_switch_system,
        zoxp_inputs_update,
        [out] inputs.DeviceModeDirty,
        [out] inputs.DeviceMode,
        [out] inputs.LastDeviceMode
    );
    /*zox_system(
        DeviceModeDirtySystem,
        zoxp_inputs_update,
        [out] DeviceModeDirty
        [out] DeviceMode,
        [out] LastDeviceMode
    );*/
}
