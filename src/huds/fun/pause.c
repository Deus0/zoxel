// from touch_ui
void button_event_pause_game(
    ecs *world,
    const ClickEventData event
) {
    entity player = event.clicker;
    entity game = zox_get_link(world, player, GameLink);
    byte game_state = zox_getv(game, GameState);
    if (!(game_state == zox_game_state_playing || game_state == zox_game_state_paused)) {
        return;
    }
    byte is_paused = game_state == zox_game_state_paused;
    zox_set(game, GameStateTarget, { is_paused ? zox_game_state_playing : zox_game_state_paused });
}
