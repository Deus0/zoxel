#include "device_mode_ui.c"
#include "terminal.c"
#include "menu_game.c"
#include "crosshair.c"

void zox_systems_game_ui(ecs *world) {
    zox_system(
        raycast_crosshair_system,
        zoxp_update,
        [in] vrays.RaycastVoxelData,
        [none] players.PlayerCharacter
    );
    zox_system(
        taskbar_begin_system,
        zoxp_initialize,
        [none] windows.Window,
        [none] core.Initialize,
    );
     // NOTE: must update before ??
    zox_system_1(
        device_mode_ui_system,
        zoxp_spawn,
        [in] inputs.DeviceModeDirty,
        [in] inputs.LastDeviceMode,
        [in] inputs.DeviceMode,
        [none] players.Player,
    );
    zox_system_1(
        player_terminal_system,
        zoxp_spawn,
        [in] inputs.DeviceMode,
        [none] players.Player
    );
}
