void button_event_end_game(ecs *world, ClickEventData event) {
    // TODO: Make this a game state change to end game
    entity player = event.clicker;
    entity game = zox_get_link(world, player, GameLink);
    if (!zox_valid(game)) {
        zox_loge("Game Invalid in end game");
        return;
    }
    zox_setv(game, GameStateTarget, zox_game_state_the_end);
    // zox_setv(game, GameStateDirty, 1);
}

void pause_resume(ecs *world, entity player) {
    entity game = zox_get_link(world, player, GameLink);
    byte game_state = zox_getv(game, GameState);
    if (!(game_state == zox_game_state_playing || game_state == zox_game_state_paused)) {
        return;
    }
    byte is_paused = game_state == zox_game_state_paused;
    if (is_paused) {
        zox_setv(game, GameStateTarget, zox_game_state_playing);
    }
}

void button_event_return_to_game(ecs *world, ClickEventData event) {
    pause_resume(world, event.clicker);
}
