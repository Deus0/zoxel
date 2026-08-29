#include "respawn.c"
#include "pause.c"
#include "game_start.c"
#include "game_start2.c"
#include "game_start3.c"
#include "game_end.c"
#include "touchui.c"
#include "actionbar.c"
#include "crosshair.c"
#include "fader.c"

void zox_define_systems_controllers3_game(ecs *world) {
    zox_system(
        PlayerTownFinderSystem,
        zoxp_update,
        [in] cameras.CameraLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system(
        PlayerPauseSystem,
        zoxp_update,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [none] players.Player
    );
    zox_system(
        PlayerScreenFadeSystem,
        zoxp_update,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] layouts.CanvasLink,
        [none] players.Player
    );
    zox_system_1(
        GameStartStreamerSystem,
        zoxp_spawn,
        [in] cameras.CameraLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system_1(
        PlayerBeginSystem,
        zoxp_spawn,
        [in] cameras.CameraLink,
        [out] characters.CharacterLink,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [none] players.Player3
    );
    zox_system_1(
        PlayerGame3EndSystem,
        zoxp_spawn,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] cameras.CameraLink,
        [out] characters.CharacterLink,
        [none] players.Player3
    );
    zox_system_1(
        Player3RespawnSystem,
        zoxp_spawn,
        [in] cameras.CameraLink,
        [out] players.PlayerStateDirty,
        [out] players.PlayerState,
        [out] characters.CharacterLink,
        [none] players.Player
    );
    /*zox_system_1(
        PlayerActionbarSystem,
        zoxp_spawn,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] layouts.CanvasLink,
        [none] players.Player3
    );
    zox_system_1(
        PlayerCrosshairSystem,
        zoxp_spawn,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] layouts.CanvasLink,
        [none] players.Player3
    );
    zox_system_1(
        PlayerTouchUISystem,
        zoxp_spawn,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] layouts.CanvasLink,
        [in] inputs.DeviceMode,
        [none] players.Player
    );*/
}

void zox_events_controllers(ecs* world) {
    zox_muter(prefab_player, PlayerStateEvent, player_event);
    add_to_PlayerStateEvent(player_event, player_state_crosshair);
    add_to_PlayerStateEvent(player_event, player_state_actionbar);
    add_to_PlayerStateEvent(player_event, player_state_touch_ui);
}


