#include "game.c"

void define_systems_controllers(ecs *world) {
    zox_system(
        PlayerGameStateSystem,
        EcsOnUpdate,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] characters.CharacterLink,
        [none] players.Player
    );
}