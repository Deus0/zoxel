void game_state_weather(ecs* world, entity game, byte state) {
    entity skybox = zox_get_link(world, game, Skybox);
    if (!zox_valid(skybox)) {
        zox_loge("Game has no skybox");
        return;
    }
    if (state == zox_game_state_play_begin) {
        set_skybox_colors(
            world,
            skybox,
            game_sky_color,
            game_sky_bottom_color);
    } else if (state == zox_game_none) {
        set_skybox_colors(
            world,
            skybox,
            menu_sky_color,
            menu_sky_bottom_color);
    }
    last_weather_state = state;
}
