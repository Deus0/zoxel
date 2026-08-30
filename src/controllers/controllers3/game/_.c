#include "respawn.c"
#include "pause.c"
#include "town_placer.c"
#include "character_spawner.c"

void zox_define_systems_controllers3_game(ecs *world) {
    zox_system(
        PlayerTownFinderSystem,
        zoxp_update,
        [in] cameras.CameraLink,
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
        [in] cameras.CameraLink,
        [out] characters.CharacterLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system_1(
        Player3RespawnSystem,
        zoxp_spawn,
        [in] cameras.CameraLink,
        [out] players.PlayerStateDirty,
        [out] players.PlayerState,
        [out] characters.CharacterLink,
        [none] players.Player
    );
    /*zox_system_1(
        GameStartStreamerSystem,
        zoxp_spawn,
        [in] cameras.CameraLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );*/
    /*zox_system_1(
        PlayerGame3EndSystem,
        zoxp_spawn,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] cameras.CameraLink,
        [out] characters.CharacterLink,
        [none] players.Player3
    );*/
}

