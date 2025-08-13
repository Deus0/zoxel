
// game
void game_state_weather(
    ecs *world,
    const entity game,
    const byte old_game_state,
    const byte new_game_state
) {
    if (old_game_state == zox_game_loading && new_game_state == zox_game_playing) {
        set_skybox_colors(world, game_sky_color, game_sky_bottom_color);
    } else if (new_game_state == zox_game_start) {
        set_skybox_colors(world, menu_sky_color, menu_sky_bottom_color);
    }
}
