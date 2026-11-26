#include "move.c"
#include "game_start.c"
#include "game_end.c"

void zox_systems_controllers2(ecs *world) {
    zox_system(
        controller2_move_system,
        zoxp_update,
        [none] players.Player
    );
    zox_on_add(player_game2_start_event, [in] players.PlayerState, [none] players.Player2, [none] core.Dirty);
    zox_on_add(player_game2_end_event, [in] players.PlayerState, [none] players.Player2, [none] core.Dirty);
}
