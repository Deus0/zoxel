#include "game.c"

void define_systems_controllers(ecs *world) {
    zox_system(
        PlayerGameStateSystem,
        EcsOnUpdate,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [none] players.Player
    );
}