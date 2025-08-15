
// game
void game_state_weather(
    ecs *world,
    const entity game,
    const byte old,
    const byte new
) {
    (void) game;
    if (old == zox_game_loading && new == zox_game_playing) {
        set_skybox_colors(world, game_sky_color, game_sky_bottom_color);
    } else if (new == zox_game_start) {
        set_skybox_colors(world, menu_sky_color, menu_sky_bottom_color);
    }
}
