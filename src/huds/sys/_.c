#include "fps_display.c"
#include "quads_display.c"
#include "device_mode_ui.c"
#include "debug_label.c"
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
        EcsOnUpdate,
        [in] vrays.RaycastVoxelData,
        [in] players.PlayerLink
    );
    zox_system(
        FpsDisplaySystem,
        EcsOnUpdate,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [out] FPSDisplayTicker,
        [none] FPSDisplay
    );
    zox_system(
        MenuGameBeginSystem,
        EcsOnUpdate,
        [in] core.InitializeEntity,
        [out] huds.TaskbarToggleLink
    );
    zox_system(
        DebugLabelSystem,
        EcsOnStore,
        [in] players.PlayerLink,
        [in] DebugLabelData,
        [out] texts.TextDirty,
        [out] texts.TextData,
        [none] huds.GameDebugLabel
    );
    zox_system(
        GameStartFaderSystem,
        EcsOnUpdate,
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
