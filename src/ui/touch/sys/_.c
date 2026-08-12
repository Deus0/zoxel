#include "joysticks.c"

void define_systems_touch(ecs *world) {
    zox_system_1(
        VirtualJoystickSystem,
        zoxp_mainthread,
        [in] inputs.DeviceLink,
        [in] raycasts.RaycasterTarget,
        [in] inputs.ZevicePointerPosition,
        [in] inputs.ZevicePointer,
        [in] inputs.VirtualZeviceLink,
        [out] elements.ElementLink,
        [none] inputs.Zevice
    );
}
