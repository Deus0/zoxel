#include "skybox.c"

void zox_events_weather(ecs *world) {
    zox_muter(prefab_game, GameStateEvent, game_event);
    add_to_GameStateEvent(game_event, game_state_weather);
}