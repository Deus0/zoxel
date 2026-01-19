// from touch_ui
void button_event_pause_game(
    ecs *world,
    const ClickEventData event
) {
    entity player = event.clicker;
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, GameState, byte, game_state);
    if (!(game_state == zox_game_playing || game_state == zox_game_paused)) {
        return;
    }
    byte is_paused = game_state == zox_game_paused;
    zox_set(game, GameStateTarget, { is_paused ? zox_game_playing : zox_game_paused });
}
