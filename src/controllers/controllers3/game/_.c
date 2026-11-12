#include "respawn.c"
#include "pause.c"
#include "town_placer.c"
#include "character_spawner.c"

void zox_systems_controllers3_game(ecs *world) {
    zox_system(
        player_town_finder_system,
        zoxp_update,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system(
        player_pause_system,
        zoxp_update,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [none] players.Player
    );
    zox_system_1(
        player_begin_system,
        zoxp_spawn,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system_1(
        player3_respawn_system,
        zoxp_spawn,
        [out] players.PlayerStateDirty,
        [out] players.PlayerState,
        [none] players.Player
    );
}

