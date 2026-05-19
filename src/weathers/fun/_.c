// game
void game_state_weather(ecs *world, entity game, byte old, byte new) {
    (void) game;
    if (new == zox_game_playing_start) {
        set_skybox_colors(world, game_sky_color, game_sky_bottom_color);
    } else if (new == zox_game_start) {
        set_skybox_colors(world, menu_sky_color, menu_sky_bottom_color);
    }
}
