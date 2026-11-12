#include "move.c"
#include "game_start.c"
#include "game_end.c"

void zox_systems_controllers2(ecs *world) {
    zox_system(
        controller2_move_system,
        zoxp_update,
        [none] players.Player
    );
    zox_system_1(
        player_game2_start_system,
        zoxp_spawn,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [none] players.Player2
    );
    zox_system_1(
        player_game2_end_system,
        zoxp_spawn,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [none] players.Player2
    );
}
