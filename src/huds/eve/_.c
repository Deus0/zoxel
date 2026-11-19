// Game
#include "fader.c"
#include "end.c"
#include "pause.c"
// Player
#include "respawn.c"
#include "load.c"
#include "town.c"
#include "disconnect.c"
// Buttons
#include "new_game.c"
#include "realm_load.c"

void zox_events_huds(ecs* world) {
    zox_muter(prefab_game, GameStateEvent, game_event);
    add_to_GameStateEvent(game_event, game_state_end_huds);
    add_to_GameStateEvent(game_event, game_state_load);
    add_to_GameStateEvent(game_event, game_state_split_screen);
    player_state_event_begin = player_ui_scale_begin;
    player_state_event_end = player_ui_scale_end;
    zox_muter(prefab_player, PlayerStateEvent, player_event);
    add_to_PlayerStateEvent(player_event, player_state_pause_ui);
    add_to_PlayerStateEvent(player_event, player_state_respawn_ui);
    zox_on_add(town_entered_toast, [none] towns.InTown, [none] players.PlayerCharacter);
    zox_on_add(disconnect_screen_show, [none] inputs.DeviceLost, [none] players.Player);
    zox_observer(disconnect_screen_hide, EcsOnRemove, [none] inputs.DeviceLost, [none] players.Player);
}
