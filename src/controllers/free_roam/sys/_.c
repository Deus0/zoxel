#include "move.c"
#include "rotate.c"
#include "toggle.c"

void define_systems_free_roam(ecs* world) {
    zox_system(
        FreeCameraMoveSystem,
        zoxp_update,
        [none] players.Player
    );
    zox_system(
        FreeCameraRotateSystem,
        zoxp_update,
        [none] players.Player
    );
    zox_system(
        FreeCameraToggleSystem,
        zoxp_update,
        [none] players.Player
    );
}
