#include "move.c"
#include "game_start.c"

void define_systems_controllers2(ecs *world) {
    zox_system(
        Controller2MoveSystem,
        EcsOnUpdate,
        [in] inputs.DeviceLinks,
        [in] characters.CharacterLink,
        [none] players.Player
    );
    zox_system_1(
        PlayerGame2StartSystem,
        EcsOnUpdate,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        // [in] games.GameLink,
        // [in] cameras.CameraLink,
        [none] players.Player
    );
}