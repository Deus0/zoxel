#include "joysticks.c"

void zox_systems_touch(ecs *world) {
    zox_system_1(
        VirtualJoystickSystem,
        zoxp_spawn,
        [in] core.EntityTarget,
        [in] inputs.ZevicePointerPosition,
        [in] inputs.ZevicePointer,
        [none] inputs.Zevice
    );
}
