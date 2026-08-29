// Game
#include "fader.c"
#include "end.c"
#include "pause.c"
// Player
#include "respawn.c"
#include "load.c"
// Buttons
#include "new_game.c"
#include "realm_load.c"

void zox_events_huds(ecs* world) {
    zox_muter(prefab_game, GameStateEvent, game_event);
    add_to_GameStateEvent(game_event, game_state_fader);
    add_to_GameStateEvent(game_event, game_state_end_huds);
    add_to_GameStateEvent(game_event, game_state_load);
    zox_muter(prefab_player, PlayerStateEvent, player_event);
    add_to_PlayerStateEvent(player_event, player_state_pause_ui);
    add_to_PlayerStateEvent(player_event, player_state_respawn_ui);
}
