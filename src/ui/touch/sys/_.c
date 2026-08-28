#include "joysticks.c"

void define_systems_touch(ecs *world) {
    zox_system_1(
        VirtualJoystickSystem,
        zoxp_spawn,
        [in] inputs.DeviceLink,
        [in] core.EntityTarget,
        [in] inputs.ZevicePointerPosition,
        [in] inputs.ZevicePointer,
        [in] inputs.VirtualZeviceLink,
        [out] ui.ElementLink,
        [none] inputs.Zevice
    );
}
