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

void define_systems_game_ui(ecs *world) {
    zox_system(
        FpsDisplaySystem,
        EcsOnUpdate,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [out] FPSDisplayTicker,
        [none] FPSDisplay
    );
    /*zox_system(QuadsLabelSystem, EcsOnUpdate,
            [out] QuadsCount,
            [out] texts.TextDirty,
            [out] texts.TextData,
            [none] QuadsCountLabel)*/
    zox_system(
        DebugLabelSystem,
        EcsOnStore,
        [in] players.PlayerLink,
        [in] DebugLabelData,
        [out] texts.TextDirty,
        [out] texts.TextData,
        [none] game.u.i.GameDebugLabel
    );
    zox_system_1(
        DeviceModeUISystem,
        EcsOnUpdate,
        [in] inputs.DeviceMode,
        [in] inputs.DeviceModeDirty,
        [in] games.GameLink,
        [in] layouts2.CanvasLink
    ); // note: must update before


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
        EcsOnUpdate,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] layouts2.CanvasLink
    );
    zox_system_1(
        PlayerUIGame3EndSystem,
        EcsOnUpdate,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] games.GameLink
    );
    zox_system_1(
        PlayerUIGamePauseSystem,
        EcsOnUpdate,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] cameras.CameraLink,
        [in] layouts2.CanvasLink,
        [out] players.PlayerPauseEvent
    );
    zox_system_1(
        PlayerUIResumeSystem,
        EcsOnUpdate,
        [in] cameras.CameraLink,
        [in] layouts2.CanvasLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [out] players.PlayerPauseEvent
    );

    zox_system_1(   // spawns ui
        PlayerTerminalSystem,
        EcsOnUpdate,
        [in] layouts2.CanvasLink,
        [in] inputs.DeviceLinks,
        [in] inputs.DeviceMode,
        [in] elements.ElementLinks,
        [none] players.Player
    );
}