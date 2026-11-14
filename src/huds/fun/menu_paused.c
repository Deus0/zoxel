// the game ends once the screen has faded out (the realm vanished, then it faded)
//  - the delay could end before the fade did (0.86 faded): waits for its fade in step to finish
void end_game_after_fade(ecs* world, entity player) {
    const entity game = zox_valid(player) ? zox_get_link(world, player, GameLink) : 0;
    if (!zox_valid(game)) {
        return;
    }
    const entity canvas = zox_get_link(world, player, CanvasLink);
    const entity overlay = zox_valid(canvas) ? zox_get_child_by_id(world, canvas, zox_id(CanvasOverlay)) : 0;
    // its first step fades in (trigger_canvas_fade_transition), then it's dark
    if (zox_valid(overlay) && zox_has(overlay, AnimationIndex) && zox_getv(overlay, AnimationIndex) == 0) {
        delay_event(world, &end_game_after_fade, player, 0.05);
        return;
    }
    zox_setv(game, GameStateTarget, zox_game_state_the_end);
}

void button_event_end_game(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity game = zox_get_link(world, player, GameLink);
    if (!zox_valid(game)) {
        zox_loge("Game Invalid in end game");
        return;
    }
    if (!is_end_game_delays) {
        zox_setv(game, GameStateTarget, zox_game_state_the_end);
        return;
    }
    // fades out, then ends (game_state_end_huds keeps it dark while the realm goes, fades the menu in)
    trigger_canvas_fade_transition(world, zox_get_link(world, player, CanvasLink), end_game_delay_fade, 1);
    delay_event(world, &end_game_after_fade, player, end_game_delay_fade);
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
