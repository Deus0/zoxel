byte last_weather_state;

// NOTE: Used for testing
void refresh_weather(ecs* world) {
    /*if (last_weather_state == zox_game_state_play_begin) {
        set_skybox_colors(world, game_sky_color, game_sky_bottom_color);
    } else if (last_weather_state == zox_game_start) {
        set_skybox_colors(world, menu_sky_color, menu_sky_bottom_color);
    }*/
}

zox_sys2(WeatherGameStateSystem) {
    // byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameStateDirty);
    zox_sys_in(GameState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GameStateDirty, dirty);
        zox_sys_i(GameState, state);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity skybox = zox_get_link(world, e, Skybox);
        if (!zox_valid(skybox)) {
            zox_loge("Game has no skybox");
            continue;
        }
        if (state->value == zox_game_state_play_begin) {
            set_skybox_colors(world, skybox, game_sky_color, game_sky_bottom_color);
        } else if (state->value == zox_game_start) {
            set_skybox_colors(world, skybox, menu_sky_color, menu_sky_bottom_color);
        }
        last_weather_state = state->value;
    }
} zox_sys_end(WeatherGameStateSystem);
