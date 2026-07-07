// game
byte last_weather_state;

void game_state_weather(ecs *world, entity game, byte old, byte new) {
    if (new == zox_game_state_play_begin) {
        set_skybox_colors(world, game_sky_color, game_sky_bottom_color);
    } else if (new == zox_game_start) {
        set_skybox_colors(world, menu_sky_color, menu_sky_bottom_color);
    }
    last_weather_state = new;
}

void refresh_weather(ecs* world) {
    game_state_weather(world, 0, 0, last_weather_state);
}