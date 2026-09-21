#include "minimap.c"

void zox_events_maps(ecs* world) {
    zox_muter(prefab_player, PlayerStateEvent, player_event);
    add_to_PlayerStateEvent(player_event, player_state_minimaps);
}