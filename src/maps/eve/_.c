#include "minimap.c"

// the player state changed (its core Dirty)
void player_state_maps_event(iter* it) {
    player_state_each(it, player_state_minimaps);
}

void zox_events_maps(ecs* world) {
    zox_on_add(player_state_maps_event, [in] players.PlayerState, [none] players.Player, [none] core.Dirty);
}