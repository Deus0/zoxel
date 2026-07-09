#include "game.c"

void zox_define_systems_controllers(ecs *world) {
    zox_system(
        ControllerPlayerStateSystem,
        EcsOnUpdate,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] characters.CharacterLink,
        [none] players.Player
    );
}
