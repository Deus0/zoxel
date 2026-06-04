#include "fps_display.c"
#include "quads_display.c"
#include "device_mode_ui.c"
#include "debug_label.c"
#include "game_start_fader.c"
#include "game_end.c"
#include "game_end3.c"
#include "game_pause.c"
#include "game_resume.c"
#include "terminal.c"
#include "menu_game.c"
#include "crosshair.c"

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
        [in] core.EntityInitialize,
        [in] layouts2.CanvasLink,
        [out] huds.TaskbarToggleLink //,
        // [none] MenuGame
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
     // NOTE: must update before ??
    zox_system_1(
        DeviceModeUISystem,
        zoxp_mainthread,
        [in] inputs.DeviceMode,
        [in] inputs.DeviceModeDirty,
        [in] games.GameLink,
        [in] layouts2.CanvasLink
    );
    zox_system(
        GameStartFaderSystem,
        EcsOnUpdate,
        [in] games.GameStateDirty,
        [in] games.GameState,
        [in] players.PlayerLinks,
        [none] games.Game
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
        PlayerUIGamePauseSystem,
        zoxp_mainthread,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] layouts2.CanvasLink,
        [in] cameras.CameraLink,
        [out] players.PlayerPauseEvent
    );
    zox_system_1(
        PlayerResumeSystem,
        zoxp_mainthread,
        [in] layouts2.CanvasLink,
        [in] cameras.CameraLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [out] players.PlayerPauseEvent
    );
    zox_system_1(   // spawns ui
        PlayerTerminalSystem,
        zoxp_mainthread,
        [in] layouts2.CanvasLink,
        [in] inputs.DeviceMode,
        [in] elements.ElementLinks,
        [none] players.Player
    );
}
