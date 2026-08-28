#include "device_mode_ui.c"
#include "game_start_fader.c"
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
        [out] huds.TaskbarToggleLink,
        [none] core.Initialize,
    );
     // NOTE: must update before ??
    zox_system_1(
        DeviceModeUISystem,
        zoxp_spawn,
        [in] inputs.DeviceModeDirty,
        [in] inputs.LastDeviceMode,
        [in] inputs.DeviceMode,
        [in] games.GameLink,
        [in] layouts.CanvasLink
    );
    zox_system_1(
        PlayerTerminalSystem,
        zoxp_spawn,
        [in] layouts.CanvasLink,
        [in] inputs.DeviceMode,
        [none] players.Player
    );
    /*zox_system(
        GameStartFaderSystem,
        zoxp_update,
        [in] games.GameStateDirty,
        [in] games.GameState,
        [in] players.PlayerLinks,
        [none] games.Game
    );
    zox_system_1(
        PlayerUIGame3EndSystem,
        zoxp_spawn,
        [in] games.GameState,
        [in] games.GameStateDirty,
        [none] games.Game
        );*/
    // Pause UI
    /*zox_system_1(
        PlayerPauseUISystem,
        zoxp_spawn,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] layouts.CanvasLink,
        [in] cameras.CameraLink,
        [none] players.Player
    );*/
    zox_system_1(
        PlayerRespawnUISystem,
        zoxp_spawn,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] layouts.CanvasLink,
        [in] cameras.CameraLink,
        [none] players.Player
    );
}

void zox_events_huds(ecs* world) {
    zox_muter(prefab_game, GameStateEvent, game_event);
    add_to_GameStateEvent(game_event, game_state_fader);
    add_to_GameStateEvent(game_event, game_state_end_huds);
    zox_muter(prefab_player, PlayerStateEvent, player_event);
    add_to_PlayerStateEvent(player_event, player_state_pause_ui);
}
