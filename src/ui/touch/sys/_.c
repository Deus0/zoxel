#include "joysticks.c"

void zox_systems_touch(ecs *world) {
    zox_system_1(
        virtual_joystick_system,
        zoxp_spawn,
        [in] core.EntityTarget,
        [in] inputs.ZevicePointerPosition,
        [in] inputs.ZevicePointer,
        [none] inputs.Zevice
    );
}
