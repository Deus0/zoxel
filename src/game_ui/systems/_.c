#include "fps_display_system.c"
#include "quads_display_system.c"
#include "device_mode_ui_system.c"
#include "debug_label_system.c"

void define_systems_game_ui(ecs *world) {
    zox_system(FpsDisplaySystem, EcsOnUpdate,
            [out] texts.TextData,
            [out] texts.TextDirty,
            [out] FPSDisplayTicker,
            [none] FPSDisplay)
    /*zox_system(QuadsLabelSystem, EcsOnUpdate,
            [out] QuadsCount,
            [out] texts.TextDirty,
            [out] texts.TextData,
            [none] QuadsCountLabel)*/
    zox_system(DebugLabelSystem, EcsOnStore,
            [in] players.PlayerLink,
            [in] DebugLabelData,
            [out] texts.TextDirty,
            [out] texts.TextData,
            [none] game.u.i.GameDebugLabel)
    zox_system_1(DeviceModeUISystem, EcsOnUpdate,
            [in] inputs.DeviceMode,
            [in] inputs.DeviceModeDirty,
            [in] games.GameLink,
            [in] layouts2.CanvasLink) // note: must update before
}