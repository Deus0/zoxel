void zoxel_settings_uis() {
    byte window_fill_alpha = 166;
    byte fill_alpha = 66;
    byte text_alpha = 172;
    byte text_outline_alpha = 222;
    byte outline_alpha = 92;
    byte outline_value = 1;
    // Windows
    window_outline = color_grayscale_a(outline_value, outline_alpha);
    window_fill = (color) { 13, 22, 16, window_fill_alpha };
    // Headers
    header_outline = color_grayscale_a(outline_value, outline_alpha);
    header_fill = (color) { 26, 44, 32, window_fill_alpha };
    header_font_outline = (color) { 158, 46, 237, text_outline_alpha };
    header_font_fill = (color) { 244, 33, 166, text_alpha };
    header_fonto_thickness = 3;
    header_font_thickness = 1;
    // Close Buttons
    close_button_outline = color_grayscale_a(outline_value, outline_alpha);
    close_button_fill = color_grayscale_a(13, fill_alpha);
    close_button_font_outline = color_grayscale_a(166, text_outline_alpha);
    close_button_font_fill = color_grayscale_a(166, text_alpha);
    close_button_fonto_thickness = 2;
    close_button_font_thickness = 2;
    // Frames
    frame_outline = color_grayscale_a(12, fill_alpha);
    frame_fill = color_grayscale_a(188, fill_alpha);
    // Buttons
    button_outline = color_grayscale_a(outline_value, fill_alpha);
    button_fill = color_grayscale_a(0, fill_alpha);
    button_font_outline = (color) { 88, 25, 124, text_outline_alpha };
    button_font_fill = (color) { 246, 2, 199, text_alpha };
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

void initialize_zoxel_settings(ecs* world) {
    profiler_logs = 0;
    target_fps = 0;
    terrain_depth = 4;
    game_ui_has_taskbar = 1;
    block_vox_render_at_lod = 0; // now using lod minimum
    // fog_density = 0.034f;
    // profiler_state = zox_profile_light_propogate;
    menu_sky_color = color_rgb_grayscale(36);
    menu_sky_bottom_color = color_rgb_grayscale(63);
    // art
    grayscale_mode = 0; // todo: make a grayscale biome
    // game
    zox_game_type = zox_game_mode_3D;
    game_rule_attach_to_character = 1;
    zox_experience_max_start = 10;
    // graphs
    zox_visualize_sounds = 0;
    // logs
    is_log_gpu_restore = 0;
    // regen_rate = 10;
    // zox_camera_state_free | zox_camera_state_first_person | zox_camera_state_third_person | zox_camera_state_ortho | zox_camera_state_topdown | zox_camera_state_2D
    zox_game_camera_mode = zox_camera_state_first_person;
    // issue: npcs can walk outside the bounds of the spawn zone atm, thus making them not be destroyed properly when moving away
    // NPCs
    is_characters_instanced = 1;
    disable_npc_hooks = 0;
    disable_npc_positioner = 0;
    // Physics
    {
        friction3D = 9; // 560;
        air_friction3D = 1; // 40;
        zox_jump_power = 4;
        run_accceleration = 2.2;
        backwards_multiplier = 0.7f;
        player_movement_power = (float2) { 14, 14 };
        max_velocity3D = (float2) { 1.3f, 1.3f };
    }
    zoxel_settings_uis();
    add_hook_key_down(toggle_fps_viewer);
    add_hook_key_down(toggle_flymode);
    add_hook_key_down(toggle_inspector);
    add_hook_key_down(toggle_hierarchy);
    add_hook_key_down(key_down_toggle_streaming);
    add_hook_key_down(key_down_toggle_npc_movement);
    // Gameplay
    // hit_terrain_color = (color) { 2, 2, 2, 255 };
    hit_character_color = (color) { 155, 45, 45, 255 };
    hit_block_vox_color = (color) { 55, 135, 185, 255 };
}

// todo: spawn_prefab_hook -> spawn after settings are set here in GameModule
// log types
// #define zox_enable_log_ios
// #define zox_enable_log_input
// #define zox_enable_log_ui
// #define zox_enable_log_shader
// set_prefab_debug_label(world, &zox_dbg_ui_canvas);
// set_prefab_debug_label(world, &debug_ui_raycast_chunk3);
// set_prefab_debug_label(world, &debug_ui_player);
// set_prefab_debug_label(world, &debug_label_chunk_link);
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
// set_prefab_debug_label(world, &debug_ui_single_npc);

