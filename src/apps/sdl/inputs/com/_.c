#include "joystick.c"
#include "gamecontroller.c"

void zox_components_sdl_inputs(ecs* world) {
    zoxd_dest_old(SdlJoystick);
    zoxd_dest_old(SdlGameController);
}
