#include "device_mode_ui.c"
#include "terminal.c"
#include "menu_game.c"
#include "crosshair.c"

void zox_systems_game_ui(ecs *world) {
    zox_system(
        RaycastCrosshairSystem,
        zoxp_update,
        [in] vrays.RaycastVoxelData,
        [none] players.PlayerCharacter
    );
    zox_system(
        TaskbarBeginSystem,
        zoxp_initialize,
        [none] windows.Window,
        [none] core.Initialize,
    );
     // NOTE: must update before ??
    zox_system_1(
        DeviceModeUISystem,
        zoxp_spawn,
        [in] inputs.DeviceModeDirty,
        [in] inputs.LastDeviceMode,
        [in] inputs.DeviceMode,
        [none] players.Player,
    );
    zox_system_1(
        PlayerTerminalSystem,
        zoxp_spawn,
        [in] inputs.DeviceMode,
        [none] players.Player
    );
}
