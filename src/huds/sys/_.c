#include "device_mode_ui.c"
#include "terminal.c"
#include "menu_game.c"
#include "crosshair.c"
#include "load3.c"
#include "navigator.c"
#include "player_join.c"

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
    // main thread: players join and leave, its ui spawns
    zox_system_1(
        player_join_system,
        zoxp_spawn,
        [in] players.PlayerState,
        [none] players.Player
    );
    zox_system_1(
        player_terminal_system,
        zoxp_spawn,
        [in] inputs.DeviceMode,
        [none] players.Player
    );
    // main thread: it writes every player's camera
    zox_system_1(load_lids_system, zoxp_update, 0);
    // main thread: it opens and closes uis
    zox_system_1(
        navigator_shift_system,
        zoxp_update,
        [in] players.PlayerState,
        [none] players.Player
    );
    // main thread: it sets settings and texts
    zox_system_1(
        huds3_options_stick_system,
        zoxp_update,
        [in] core.EntityTarget,
        [none] ui.NavigatorState
    );
    // main thread: it reads the camera
    zox_system_1(
        huds3_follow_system,
        zoxp_update,
        [none] huds.LoadUI,
        [none] layouts.Canvas3
    );
}
