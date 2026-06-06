#include "move.c"
#include "game_start.c"
#include "game_end.c"

void define_systems_controllers2(ecs *world) {
    zox_system(
        Controller2MoveSystem,
        EcsOnUpdate,
        [in] characters.CharacterLink,
        [none] players.Player
    );
    zox_system_1(
        PlayerGame2StartSystem,
        EcsOnUpdate,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [none] players.Player2
    );
    zox_system_1(
        PlayerGame2EndSystem,
        EcsOnUpdate,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [out] characters.CharacterLink,
        [none] players.Player2
    );
}
