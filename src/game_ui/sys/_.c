#include "fps_display.c"
#include "quads_display.c"
#include "device_mode_ui.c"
#include "debug_label.c"
#include "game_start.c"

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
}