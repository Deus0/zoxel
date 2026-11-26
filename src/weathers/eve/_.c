#include "skybox.c"

// the game's state changed (its core Dirty): the sky's colours
void game_state_weather_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GameState, state);
        game_state_weather(world, e, state->value);
    }
}

void zox_events_weather(ecs *world) {
    zox_on_add(game_state_weather_event, [in] games.GameState, [none] games.Game, [none] core.Dirty);
}