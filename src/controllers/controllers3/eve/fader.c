void player_state_fader(ecs* world, entity player, byte state) {
    double game_load_fade_transition_time = 0.5;
    double game_spawn_terrain_fade_delay = 1.4;
    entity canvas = zox_get_link(world, player, Canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    if (state == zox_player_state_starting) {
        trigger_canvas_fade_out(
            world,
            canvas,
            game_spawn_terrain_fade_delay,
            game_load_fade_transition_time);
    }
}
