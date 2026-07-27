#include "device_mode_ui.c"
#include "game_start_fader.c"
#include "game_end.c"
#include "game_end3.c"
#include "game_pause.c"
#include "terminal.c"
#include "menu_game.c"
#include "crosshair.c"
#include "respawn.c"

void define_systems_game_ui(ecs *world) {
    zox_system(
        RaycastCrosshairSystem,
        zoxp_update,
        [in] vrays.RaycastVoxelData,
        [in] players.PlayerLink
    );
    zox_system(
        MenuGameBeginSystem,
        zoxp_update,
        [in] core.Initialize,
        [out] huds.TaskbarToggleLink
    );
    zox_system(
        GameStartFaderSystem,
        zoxp_update,
        [in] games.GameStateDirty,
        [in] games.GameState,
        [in] players.PlayerLinks,
        [none] games.Game
    );
     // NOTE: must update before ??
    zox_system_1(
        DeviceModeUISystem,
        zoxp_mainthread,
        [in] inputs.DeviceModeDirty,
        [in] inputs.LastDeviceMode,
        [in] inputs.DeviceMode,
        [in] games.GameLink,
        [in] layouts2.CanvasLink
    );
    zox_system_1(
        PlayerUIGameEndSystem,
        zoxp_mainthread,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] layouts2.CanvasLink
    );
    zox_system_1(
        PlayerUIGame3EndSystem,
        zoxp_mainthread,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] games.GameLink
    );
    // Pause UI
    zox_system_1(
        PlayerPauseUISystem,
        zoxp_mainthread,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] layouts2.CanvasLink,
        [in] cameras.CameraLink,
        [none] players.Player
    );
    zox_system_1(
        PlayerRespawnUISystem,
        zoxp_mainthread,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] layouts2.CanvasLink,
        [in] cameras.CameraLink,
        [none] players.Player
    );
    zox_system_1(
        PlayerTerminalSystem,
        zoxp_mainthread,
        [in] layouts2.CanvasLink,
        [in] inputs.DeviceMode,
        [none] players.Player
    );
}
