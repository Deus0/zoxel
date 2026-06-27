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
    zox_system_1(
        PlayerActionbarSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system_1(
        PlayerCrosshairSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] layouts2.CanvasLink,
        [none] players.Player3
    );
    zox_system_1(
        PlayerTouchUISystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] layouts2.CanvasLink,
        [in] inputs.DeviceMode,
        [none] players.Player
    );
    zox_system_1(
        GameStartStreamerSystem,
        EcsOnUpdate,
        [in] cameras.CameraLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system(
        PlayerTownFinderSystem,
        EcsOnUpdate,
        [in] cameras.CameraLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system_1(
        PlayerBeginSystem,
        EcsOnUpdate,
        [in] cameras.CameraLink,
        [in] characters.CharacterLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system_1(
        PlayerGame3EndSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] cameras.CameraLink,
        [in] characters.CharacterLink,
        [none] players.Player3
    );
    zox_system_1(
        Player3RespawnSystem,
        EcsOnUpdate,
        [in] cameras.CameraLink,
        [out] players.PlayerStateDirty,
        [out] players.PlayerState,
        [out] characters.CharacterLink,
        [none] players.Player
    );
    zox_system(
        PlayerPauseSystem,
        EcsOnUpdate,
        [in] players.PlayerState,
        [in] games.GameLink,
        [none] players.Player
    );
}
