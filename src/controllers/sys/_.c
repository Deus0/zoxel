#include "game.c"

void zox_systems_controllers(ecs *world) {
    zox_system(
        ControllerPlayerStateSystem,
        zoxp_update,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [none] players.Player
    );
}
