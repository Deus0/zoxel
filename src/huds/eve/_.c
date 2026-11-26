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

// the game's state changed (its core Dirty): the end, load, split screen and pause uis
void game_state_huds_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GameState, state);
        game_state_end_huds(world, e, state->value);
        game_state_load(world, e, state->value);
        game_state_split_screen(world, e, state->value);
        game_state_pause_menu(world, e, state->value);
    }
}

// the player state changed (its core Dirty)
void player_state_huds_event(iter* it) {
    player_state_each(it, player_state_pause_ui);
    player_state_each(it, player_state_respawn_ui);
}

void zox_events_huds(ecs* world) {
    zox_on_add(game_state_huds_event, [in] games.GameState, [none] games.Game, [none] core.Dirty);
    zox_on_add(player_state_huds_event, [in] players.PlayerState, [none] players.Player, [none] core.Dirty);
    zox_on_add(town_entered_toast, [none] towns.InTown, [none] players.PlayerCharacter);
    zox_on_add(disconnect_screen_show, [none] inputs.DeviceLost, [none] players.Player);
    zox_observer(disconnect_screen_hide, EcsOnRemove, [none] inputs.DeviceLost, [none] players.Player);
}
