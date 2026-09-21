#include "reenable.c"
#include "reset.c"
#include "device_mode.c"

void zox_systems_input(ecs* world) {
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
        [none] !inputs.Finger,
        [none] core.Disabled,
    );
    zox_system(
        ZeviceStickEnableSystem,
        zoxp_inputs_enable,
        [in] inputs.ZeviceStick,
        [none] !inputs.Finger,
        [none] core.Disabled,
    );
    zox_system(
        ZevicePointerEnableSystem,
        zoxp_inputs_enable,
        [in] inputs.ZevicePointer,
        [none] !inputs.Finger,
        [none] core.Disabled,
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
