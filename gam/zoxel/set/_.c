// todo: spawn_prefab_hook -> spawn after settings are set here in GameModule
// log types
// #define zox_enable_log_ios
// #define zox_enable_log_input
// #define zox_enable_log_ui
// #define zox_enable_log_shader

// TODO: Just add these in systems onto the DEBUG UI hook - TOggle Between key
void zoxel_on_spawn_prefabs(ecs* world) {
    // set_prefab_debug_label(world, &zox_dbg_ui_canvas);
    // set_prefab_debug_label(world, &debug_ui_raycast_chunk3);
    // set_prefab_debug_label(world, &debug_ui_player);
    set_prefab_debug_label(world, &debug_label_chunk_link);
    // set_prefab_debug_label(world, &debug_ui_raycasting);
    // set_prefab_debug_label(world, &debug_ui_system_times);
    // get_debug_label_app
    // set_prefab_debug_label(world, &debug_ui_memory);
    // set_prefab_debug_label(world, &get_label_realm_colors);
    // set_prefab_debug_label(world, &get_label_player_element_links);
    // set_prefab_debug_label(world, &get_label_player_stats);
    // set_prefab_debug_label(world, &get_label_player_items);
    // set_prefab_debug_label(world, &get_label_player_skills);
    // set_prefab_debug_label(world, &get_label_player_actions);
    // set_prefab_debug_label(world, &get_label_player_quests);
    // set_prefab_debug_label(world, &debug_label_lods);
    // set_prefab_debug_label(world, &debug_label_collisions);
    // set_prefab_debug_label(world, &debug_label_app);
    // set_prefab_debug_label(world, &debug_ui_raycasted_light);
    // set_prefab_debug_label(world, &debug_ui_single_npc);
}


void zoxel_set_debug() {
    profiler_state = zox_profile_light_propogate;
    profiler_logs = 0;
    disable_block_vox_generation = 0;
}

void zoxel_settings_npcs() {
    // issue: npcs can walk outside the bounds of the spawn zone atm, thus making them not be destroyed properly when moving away
    // debug
    is_characters_instanced = 1;
    disable_npc_hooks = 0;
    disable_npc_uis = 0;
    disable_npc_positioner = 0;
    is_paint_skeletons = 0;
    /*
    character_spawn_rate = 128;
    is_characters_instanced = 1;
    disable_npc_hooks = 1;
    disable_npc_uis = 1;
    */
}

void zoxel_settings_physics() {
    // physics
    // target_fps = 15;
    friction3D = 9; // 560;
    air_friction3D = 1; // 40;
    bounce_lost_force = 0.12f;
    // zox_default_gravity.y = -8;
    zox_jump_power = 4;
    run_accceleration = 2.2;
    backwards_multiplier = 0.7f;
    player_movement_power = (float2) { 14, 14 };
    max_velocity3D = (float2) { 1.3f, 1.3f };
    // run_speed = 1.36;
    // run_speed = 0.3;
}

void zoxel_settings_uis() {
    byte window_fill_alpha = 166;
    byte fill_alpha = 66;
    byte text_alpha = 122;
    byte outline_alpha = 92;
    byte outline_value = 1;
    // Windows
    window_outline = color_grayscale_a(outline_value, outline_alpha);
    window_fill = (color) { 13, 22, 16, window_fill_alpha };
    // Headers
    header_outline = color_grayscale_a(outline_value, outline_alpha);
    header_fill = (color) { 26, 44, 32, window_fill_alpha };
    header_font_outline = (color) { 158, 46, 237, text_alpha };
    header_font_fill = (color) { 244, 33, 166, text_alpha };
    header_fonto_thickness = 3;
    header_font_thickness = 1;
    // Close Buttons
    close_button_outline = color_grayscale_a(outline_value, outline_alpha);
    close_button_fill = color_grayscale_a(13, fill_alpha);
    close_button_font_outline = color_grayscale_a(166, text_alpha);
    close_button_font_fill = color_grayscale_a(166, text_alpha);
    close_button_fonto_thickness = 2;
    close_button_font_thickness = 2;
    // Frames
    frame_outline = color_grayscale_a(12, fill_alpha);
    frame_fill = color_grayscale_a(188, fill_alpha);
    // Buttons
    button_outline = color_grayscale_a(outline_value, fill_alpha);
    button_fill = color_grayscale_a(0, fill_alpha);
    button_font_outline = (color) { 188, 75, 244, text_alpha };
    button_font_fill = (color) { 226, 2, 133, text_alpha };
    button_font_thickness_fill = 2;
    button_font_thickness_outline = 3;
    button_fill_selected = (color) { 32, 88, 122, fill_alpha };
    button_outline_selected = (color) { 226, 2, 133, outline_alpha };
    button_outline_active = (color) { 100, 244, 177, text_alpha };
    // Handles
    handle_outline = color_grayscale_a(122, outline_alpha);
    handle_fill = color_grayscale_a(77, fill_alpha);
    // ?
    default_fill_color = color_grayscale_a(2, fill_alpha);
    default_outline_color = header_outline;
    // Tooltips
    tooltip_fillo = button_fill; // color_grayscale_a(222, outline_alpha);
    tooltip_fill = button_fill; // color_grayscale_a(0, fill_alpha);
    tooltip_font_fillo = button_font_outline; // color_grayscale_a(160, outline_alpha);
    tooltip_font_fill = color_grayscale_a(244, fill_alpha);
    tooltip_fillo.a = 222;
    tooltip_fill.a = 222;
    tooltip_font_fillo.a = 222;
    tooltip_font_fill.a = 222;
    // fades
    is_start_game_delays = 1;
    is_end_game_delays = 1;
}

void zoxel_debug_keys() {
    add_hook_key_down(toggle_fps_viewer);
    add_hook_key_down(toggle_flymode);
    add_hook_key_down(toggle_inspector);
    add_hook_key_down(toggle_hierarchy);
    add_hook_key_down(key_down_toggle_streaming);
    add_hook_key_down(key_down_toggle_npc_movement);
    add_hook_key_down(key_down_toggle_debug_chunks);
}

void initialize_zoxel_settings(ecs* world) {
    float viewport_downscale = 1;
    viewport_scale = 1 / viewport_downscale;
    target_fps = 0;
    zoxel_set_debug();
    render_distance_y = 4;
    // art
    grayscale_mode = 0; // todo: make a grayscale biome
    // game
    zox_game_type = zox_game_mode_3D;
    game_rule_attach_to_character = 1;
    zox_experience_max_start = 10;
    // regen_rate = 10;
    // zox_camera_state_free | zox_camera_state_first_person | zox_camera_state_third_person | zox_camera_state_ortho | zox_camera_state_topdown | zox_camera_state_2D
    zox_game_camera_mode = zox_camera_state_first_person;
    zoxel_settings_npcs();
    zoxel_settings_physics();
    zoxel_settings_uis();
    zoxel_debug_keys();
    // graphs
    zox_visualize_sounds = 0;
    // logs
    is_log_gpu_restore = 0;
    // Gameplay
    // hit_terrain_color = (color) { 2, 2, 2, 255 };
    hit_character_color = (color) { 155, 45, 45, 255 };
    hit_block_vox_color = (color) { 55, 135, 185, 255 };
    // world gen
    // terrain_frequency = 0.062216;
    // block_spawn_chance_grass = 1600; //  512 | 1024 | 2048 | 3000
    menu_sky_color = color_rgb_grayscale(111);
    menu_sky_bottom_color = color_rgb_grayscale(66);
    // set_camera_fog_color(world, menu_sky_color);
    // viewport_clear_color = menu_sky_color;
    block_vox_render_at_lod = 0; // now using lod minimum
    fog_density = 0.034f;
    // scaling
    terrain_lod_near = 3;
    terrain_depth = 4;
    game_ui_has_taskbar = 1;
    // zox_debug_id(Block)
    // zox_debug_print_modules();
    // zox_debug_print_systems(world);
    // zox_debug_print_components();
}
