#include "respawn.c"
#include "pause.c"
#include "town_placer.c"
#include "character_spawner.c"

void zox_systems_controllers3_game(ecs *world) {
    zox_system(
        PlayerTownFinderSystem,
        zoxp_update,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system(
        PlayerPauseSystem,
        zoxp_update,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [none] players.Player
    );
    zox_system_1(
        PlayerBeginSystem,
        zoxp_spawn,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system_1(
        Player3RespawnSystem,
        zoxp_spawn,
        [out] players.PlayerStateDirty,
        [out] players.PlayerState,
        [none] players.Player
    );
}

