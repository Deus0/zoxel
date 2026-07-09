#include "respawn.c"
#include "pause.c"
#include "game_start.c"
#include "game_start2.c"
#include "game_start3.c"
#include "game_end.c"
#include "touchui.c"
#include "actionbar.c"
#include "crosshair.c"

void zox_define_systems_controllers3_game(ecs *world) {
    zox_system(
        PlayerTownFinderSystem,
        EcsOnUpdate,
        [in] cameras.CameraLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system(
        PlayerPauseSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] games.GameLink,
        [none] players.Player
    );
    zox_system_1(
        PlayerActionbarSystem,
        zoxp_mainthread,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] layouts2.CanvasLink,
        [none] players.Player3
    );
    zox_system_1(
        PlayerCrosshairSystem,
        zoxp_mainthread,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] layouts2.CanvasLink,
        [none] players.Player3
    );
    zox_system_1(
        PlayerTouchUISystem,
        zoxp_mainthread,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] layouts2.CanvasLink,
        [in] inputs.DeviceMode,
        [none] players.Player
    );
    zox_system_1(
        GameStartStreamerSystem,
        zoxp_mainthread,
        [in] cameras.CameraLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system_1(
        PlayerBeginSystem,
        zoxp_mainthread,
        [in] cameras.CameraLink,
        [out] characters.CharacterLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system_1(
        PlayerGame3EndSystem,
        zoxp_mainthread,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] cameras.CameraLink,
        [in] characters.CharacterLink,
        [none] players.Player3
    );
    zox_system_1(
        Player3RespawnSystem,
        zoxp_mainthread,
        [in] cameras.CameraLink,
        [out] players.PlayerStateDirty,
        [out] players.PlayerState,
        [out] characters.CharacterLink,
        [none] players.Player
    );
}
