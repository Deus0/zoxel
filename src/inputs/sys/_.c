#include "zevice_button_reset.c"
#include "zevice_pointer_reset.c"
#include "zevice_pointer_delta_reset.c"
#include "zevice_button_enable.c"
#include "zevice_stick_enable.c"
#include "zevice_pointer_enable.c"
#include "device_mode_dirty.c"
#include "keyboard_reset.c"
#include "zevice_pointer_right_reset_button.c"
#include "zevice_finger_reset_button.c"
#include "device_mode.c"

void define_systems_input(ecs* world) {
    // resets
    zox_system(
        ZevicePointerResetSystem,
        zoxp_inputs_reset,
        [out] inputs.ZevicePointer,
        [out] inputs.ZevicePointerOld,
        [none] !inputs.Finger
    );
    zox_system(
        ZeviceButtonResetSystem,
        zoxp_inputs_reset,
        [out] inputs.ZeviceButton,
        [none] !inputs.Finger
    );
    zox_system(
        ZeviceButtonEnableSystem,
        zoxp_inputs_enable,
        [in] inputs.ZeviceButton,
        [out] inputs.ZeviceDisabled,
        [none] !inputs.Finger
    );
    zox_system(
        ZeviceStickEnableSystem,
        zoxp_inputs_enable,
        [in] inputs.ZeviceStick,
        [out] inputs.ZeviceDisabled,
        [none] !inputs.Finger
    );
    zox_system(
        ZevicePointerEnableSystem,
        zoxp_inputs_enable,
        [in] inputs.ZevicePointer,
        [out] inputs.ZeviceDisabled,
        [none] !inputs.Finger
    );
    /*zox_system(
        ZeviceFingerResetSystem,
        zoxp_inputs_reset,
        [in] ZevicePointerOld,
        [out] ZevicePointerPosition,
        [out] ZevicePointerDelta,
        [none] Finger
    );*/
    zox_system(
        ZevicePointerRightResetSystem,
        zoxp_inputs_reset,
        [out] inputs.ZevicePointerRight
    );
    zox_system(
        ZevicePointerDeltaResetSystem,
        zoxp_inputs_reset,
        [out] inputs.ZevicePointerDelta
    );
    zox_system(
        KeyboardResetSystem,
        zoxp_inputs_reset,
        [out] inputs.Keyboard
    );
    // state changes
    zox_system(
        DeviceSwitchSystem,
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
