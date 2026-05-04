#ifndef zoxm_game
#define zoxm_game

zox_begin_module(ZoxGame)
    game_name = "zixel";
    zox_game_type = zox_game_mode_2D;
    zox_game_camera_mode = zox_camera_state_2D;
    menu_sky_color = (color_rgb) { 33, 33, 166 };
    menu_sky_bottom_color = (color_rgb) { 33, 33, 166 };
    game_sky_color = (color_rgb) { 22, 177, 33 };
    game_sky_bottom_color = (color_rgb) { 22, 177, 33 };
    viewport_clear_color = menu_sky_bottom_color;
    // set_prefab_debug_label(world, &get_label_character2_player);
    // add_hook_on_boot(spawn_weather);
zox_end_module(ZoxGame)

#endif
