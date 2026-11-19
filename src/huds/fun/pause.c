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

// a player's own game menus (the taskbar and its uis) opened or closed, the game going on: Y, R, a touch button
//  - not while the game's paused (its pause menu's up) or the player's between states
void toggle_game_menus(ecs* world, entity player) {
    const entity game = zox_get_link(world, player, GameLink);
    if (!zox_valid(game) || zox_getv(game, GameState) != zox_game_state_playing || zox_getv(player, PlayerStateDirty)) {
        return;
    }
    const byte state = zox_getv(player, PlayerState);
    if (state != zox_player_state_playing && state != zox_player_state_paused) {
        return;
    }
    zox_setv(player, PlayerState, state == zox_player_state_paused ? zox_player_state_resume_begin : zox_player_state_pause_begin);
    zox_setv(player, PlayerStateDirty, zox_dirty_trigger);
}

void button_event_game_menus(ecs* world, const ClickEventData event) {
    toggle_game_menus(world, event.clicker);
}
