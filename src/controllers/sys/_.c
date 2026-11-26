#include "game.c"

void zox_systems_controllers(ecs *world) {
    zox_on_add(controller_player_state_event, [in] players.PlayerState, [none] players.Player, [none] core.Dirty);
}
