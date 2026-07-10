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
        [out] ZevicePointer,
        [out] ZevicePointerOld
    );
    zox_system(
        ZeviceButtonResetSystem,
        zoxp_inputs_reset,
        [out] ZeviceButton
    );
    zox_system(
        ZeviceFingerResetSystem,
        zoxp_inputs_reset,
        [in] ZevicePointerOld,
        [out] ZevicePointerPosition,
        [out] ZevicePointerDelta,
        [none] Finger
    );
    zox_system(
        ZevicePointerRightResetSystem,
        zoxp_inputs_reset,
        [out] ZevicePointerRight
    );
    zox_system(
        ZevicePointerDeltaResetSystem,
        zoxp_inputs_reset,
        [out] ZevicePointerDelta
    );
    zox_system(
        KeyboardResetSystem,
        zoxp_inputs_reset,
        [out] Keyboard
    );
    // state changes
    zox_system(
        DeviceSwitchSystem,
        zoxp_inputs_update,
        [out] DeviceModeDirty,
        [out] DeviceMode,
        [out] LastDeviceMode
    );
    /*zox_system(
        DeviceModeDirtySystem,
        zoxp_inputs_update,
        [out] DeviceModeDirty
        [out] DeviceMode,
        [out] LastDeviceMode
    );*/
    zox_system(
        ZeviceButtonEnableSystem,
        zoxp_inputs_enable,
        [in] ZeviceButton,
        [out] ZeviceDisabled
    );
    zox_system(
        ZeviceStickEnableSystem,
        zoxp_inputs_enable,
        [in] ZeviceStick,
        [out] ZeviceDisabled
    );
    zox_system(
        ZevicePointerEnableSystem,
        zoxp_inputs_enable,
        [in] ZevicePointer,
        [out] ZeviceDisabled
    );
}
