#include "joystick.c"
#include "gamecontroller.c"

void zox_define_components_sdl_inputs(ecs* world) {
    zoxd_dest(SdlJoystick);
    zoxd_dest(SdlGameController);
}