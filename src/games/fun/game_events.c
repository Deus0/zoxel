// an example of a zox_game_event can be a function that grabs all players and spawns them a character
/*zox_game_event_array_d* game_state_event;

void initialize_events_games() {
    game_state_event = create_zox_game_event_array_d(initial_dynamic_array_size);
}

void dispose_events_games() {
    dispose_zox_game_event_array_d(game_state_event);
}

// TODO: Remove this
void add_to_event_game_state(zox_game_event funn) {
    zox_game_event_array_d_add(game_state_event, funn);
}

void trigger_event_game(ecs* world, entity game, byte old_game_state, byte new_game_state) {
    for (size_t i = 0; i < game_state_event->size; i++) {
        if (game_state_event->data[i].value) (*game_state_event->data[i].value)(world, game, old_game_state, new_game_state);
    }
}
*/
