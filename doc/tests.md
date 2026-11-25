# Full test run results

## 2026-11-26

Ran `bash bsh/tst.sh all` on Windows at commit `3a5204566`.
**417 of 433 checks passed and 16 failed.** System lint and all four build checks passed.
The full run took about **1 hour**, including the initial debug build.
From test discovery to the last runtime result, the logs span **58 min 31 s**.

### Follow-up timings

| Test | Full-run baseline | Follow-up | Result |
| --- | ---: | ---: | --- |
| `lint_builds` | 319.84 s | ~28 s | Both Windows builds passed |
| `block_break_streaming` | 85.42 s | 14.85 s | 3/3 runs passed, 100 checks each |
| `aesthetic_particles` | 78.77 s | 11.74 s | On-demand tool passed, 35 frames and six bursts |
| `aesthetic_town` | 71.83 s | 78.14 s | On-demand tool passed, all three screenshots saved |

The updated Windows build check compiles and links OpenGL and Vulkan with `build.sh --check`.
The streaming test's follow-up time is the last run including engine startup. Across the three
runs, the time from the test's start to its pass was 17.14 s, 14.26 s, and 14.30 s.
The README captures now run on demand via `bsh/tools/view_particles.sh` and
`bsh/tools/view_town.sh`. Their combined baseline was 150.60 s. The particle tool waits
for the capture camera and terrain to settle. Its opening frames were checked for
terrain popping in.
The table below records the original full run.

### Failures

- `editor` (60.01 s). Log `tst/editor_failed_020421.log`.
- `block_break_player` (39.57 s). Log `tst/block_break_player_failed_014119.log`.
- `furniture_place` (29.36 s). Log `tst/furniture_place_failed_014325.log`.
- `lod_light_flash` (23.08 s). Log `tst/lod_light_flash_failed_012339.log`.
- `feet_planted` (15.50 s). Log `tst/feet_planted_failed_013303.log`.
- `navigator` (14.07 s). Log `tst/navigator_failed_013851.log`.
- `navigator_inventory` (13.47 s). Log `tst/navigator_inventory_failed_020007.log`.
- `gpu_restore_game` (13.33 s). Log `tst/gpu_restore_game_failed_011440.log`.
- `realm_pause` (13.02 s). Log `tst/realm_pause_failed_013247.log`.
- `walk_bob` (12.39 s). Log `tst/walk_bob_failed_015905.log`.
- `home_foundations` (12.00 s). Log `tst/home_foundations_failed_012710.log`.
- `resume_no_punch` (7.51 s). Log `tst/resume_no_punch_failed_020238.log`.
- `xr_lean` (7.32 s). Log `tst/xr_lean_failed_020451.log`.
- `xr_play` (6.45 s). Log `tst/xr_play_failed_020431.log`.
- `gpu_restore` (1.17 s). Log `tst/gpu_restore_failed_011425.log`.
- `camera_filter` (0.66 s). Log `tst/camera_filter_failed_011456.log`.

### Per-test timings

Sorted by duration, longest first. Runtime timings span the first and last timestamped
lines in each game log and include game startup and readiness waits. They exclude
runner overhead outside the game log. Lint timings are estimates from log modification times.

| Test | Seconds | Result |
| --- | ---: | --- |
| `lint_builds` | 319.84 | Pass |
| `block_break_streaming` | 85.42 | Pass |
| `aesthetic_particles` | 78.77 | Pass |
| `aesthetic_town` | 71.83 | Pass |
| `editor` | 60.01 | Fail |
| `npcs_after_load` | 53.76 | Pass |
| `end_game_cleanup` | 49.98 | Pass |
| `stream_npcs` | 48.05 | Pass |
| `block_break_player` | 39.57 | Fail |
| `grass_overlap` | 38.30 | Pass |
| `coop_respawn_navigator` | 32.94 | Pass |
| `perf_fly` | 32.04 | Pass |
| `bed_respawn` | 31.98 | Pass |
| `lod_swap` | 29.59 | Pass |
| `furniture_place` | 29.36 | Fail |
| `player_respawn` | 24.81 | Pass |
| `stream_sides` | 24.45 | Pass |
| `coop_respawn` | 24.12 | Pass |
| `stream_seams` | 23.60 | Pass |
| `lod_light_flash` | 23.08 | Fail |
| `chunk_mesh_flicker` | 22.32 | Pass |
| `quest_dialogue` | 21.06 | Pass |
| `menu_flow` | 18.47 | Pass |
| `coop_navigator` | 18.30 | Pass |
| `perf_idle` | 18.16 | Pass |
| `bed_sleep` | 17.89 | Pass |
| `perf_stream` | 17.75 | Pass |
| `swim` | 17.54 | Pass |
| `swing_motion` | 17.44 | Pass |
| `game_log` | 17.39 | Pass |
| `coop_map_markers` | 17.09 | Pass |
| `tables_stream` | 17.04 | Pass |
| `monster_leak` | 16.70 | Pass |
| `humanoid_leak` | 16.70 | Pass |
| `screenshot` | 16.63 | Pass |
| `shoot_burn` | 16.44 | Pass |
| `stream_gpu_sides` | 16.07 | Pass |
| `punch_wind_up` | 15.92 | Pass |
| `water_flow` | 15.87 | Pass |
| `feet_planted` | 15.50 | Fail |
| `glow_light` | 15.46 | Pass |
| `lamp_light` | 15.44 | Pass |
| `castbar` | 15.18 | Pass |
| `perf_terrain` | 15.17 | Pass |
| `town_view` | 15.04 | Pass |
| `dialogue_aim` | 14.80 | Pass |
| `sky_clouds` | 14.77 | Pass |
| `npc_leash` | 14.73 | Pass |
| `guard_steady` | 14.53 | Pass |
| `coop_join_continue` | 14.53 | Pass |
| `lod_chunk_lights` | 14.46 | Pass |
| `underwater` | 14.19 | Pass |
| `statbars_hide` | 14.16 | Pass |
| `navigator` | 14.07 | Fail |
| `coop_homes` | 14.01 | Pass |
| `continue_game` | 13.62 | Pass |
| `navigator_inventory` | 13.47 | Fail |
| `gpu_restore_game` | 13.33 | Fail |
| `door_light` | 13.31 | Pass |
| `coop_continue` | 13.30 | Pass |
| `spawn_particles` | 13.30 | Pass |
| `eat_swing` | 13.24 | Pass |
| `arms_in_step` | 13.20 | Pass |
| `walk_views` | 13.04 | Pass |
| `realm_pause` | 13.02 | Fail |
| `ui3_occlusion` | 12.96 | Pass |
| `punch_views` | 12.88 | Pass |
| `water_light` | 12.86 | Pass |
| `head_camera` | 12.83 | Pass |
| `mirror_hat` | 12.81 | Pass |
| `find_quest` | 12.61 | Pass |
| `town_furniture` | 12.58 | Pass |
| `slime_pounce` | 12.52 | Pass |
| `home_glow` | 12.47 | Pass |
| `walk_bob` | 12.39 | Fail |
| `arm_reach` | 12.32 | Pass |
| `slime_tentacle_wall` | 12.24 | Pass |
| `all_items` | 12.21 | Pass |
| `split_screen_game` | 12.21 | Pass |
| `music_crossfade` | 12.19 | Pass |
| `xr_navigator` | 12.17 | Pass |
| `bone_rebuild` | 12.09 | Pass |
| `crouch` | 12.01 | Pass |
| `home_foundations` | 12.00 | Fail |
| `gravity_cube_vode` | 11.96 | Pass |
| `monster_sight` | 11.95 | Pass |
| `npc_skill_switch` | 11.94 | Pass |
| `town_homes` | 11.93 | Pass |
| `highways` | 11.82 | Pass |
| `door_rays` | 11.82 | Pass |
| `quest_marker` | 11.79 | Pass |
| `particles_render` | 11.78 | Pass |
| `touch_actions` | 11.77 | Pass |
| `place_hit` | 11.72 | Pass |
| `region_population` | 11.68 | Pass |
| `doors` | 11.62 | Pass |
| `home_roofs` | 11.61 | Pass |
| `gamepad_reconnect_keyboard` | 11.37 | Pass |
| `action_wheel` | 11.32 | Pass |
| `npc_flee_hurt` | 11.29 | Pass |
| `touch_play` | 11.16 | Pass |
| `split_screen_huds` | 11.16 | Pass |
| `block_vox_hits` | 11.16 | Pass |
| `load_lids` | 11.05 | Pass |
| `map_pixel` | 11.05 | Pass |
| `window_toggle` | 11.02 | Pass |
| `town_enter` | 10.88 | Pass |
| `slime_tentacle` | 10.84 | Pass |
| `town_maps` | 10.83 | Pass |
| `aura_poison` | 10.76 | Pass |
| `tooltip_flick` | 10.64 | Pass |
| `vegetation_chunks` | 10.62 | Pass |
| `ui3_overlap` | 10.55 | Pass |
| `sky_blend` | 10.53 | Pass |
| `xr_continue` | 10.46 | Pass |
| `xr_frustum` | 10.45 | Pass |
| `coop_billboards` | 10.44 | Pass |
| `guard_pitch` | 10.30 | Pass |
| `coop_gamepad_reconnect` | 10.30 | Pass |
| `xr_body` | 10.24 | Pass |
| `region_zones` | 10.13 | Pass |
| `coop_pause` | 10.12 | Pass |
| `coop_streamers` | 10.05 | Pass |
| `door_sky` | 10.02 | Pass |
| `glow_relight` | 9.98 | Pass |
| `held_block` | 9.93 | Pass |
| `lod_flyby` | 9.87 | Pass |
| `oil_flow` | 9.83 | Pass |
| `held_item` | 9.82 | Pass |
| `villager_guard` | 9.77 | Pass |
| `split_screen` | 9.76 | Pass |
| `auto_jump` | 9.63 | Pass |
| `biome_map` | 9.63 | Pass |
| `pipeline_syncs` | 9.58 | Pass |
| `id_window` | 9.56 | Pass |
| `furniture_rotation` | 9.56 | Pass |
| `dig_light` | 9.39 | Pass |
| `npc_attacks` | 9.36 | Pass |
| `block_vox_lights` | 9.22 | Pass |
| `block_table` | 9.14 | Pass |
| `town_maps_empty` | 9.09 | Pass |
| `xr_actions` | 9.04 | Pass |
| `two_streamers` | 9.01 | Pass |
| `player_spawn` | 8.97 | Pass |
| `coop_block_crack` | 8.75 | Pass |
| `coop_skybox` | 8.68 | Pass |
| `block_break` | 8.68 | Pass |
| `navigator_grid` | 8.52 | Pass |
| `quest_log_tracked` | 8.33 | Pass |
| `clothes` | 8.25 | Pass |
| `step_gap` | 8.22 | Pass |
| `sea_level` | 8.21 | Pass |
| `footsteps` | 8.11 | Pass |
| `punch_motion` | 7.89 | Pass |
| `coop_saves` | 7.86 | Pass |
| `bed_wake_pad` | 7.86 | Pass |
| `xr_look_down` | 7.75 | Pass |
| `body_weights` | 7.75 | Pass |
| `body_poses` | 7.74 | Pass |
| `ik_poses` | 7.73 | Pass |
| `held_view` | 7.70 | Pass |
| `npc_stats` | 7.60 | Pass |
| `water_splash` | 7.54 | Pass |
| `resume_no_punch` | 7.51 | Fail |
| `lakes` | 7.46 | Pass |
| `gravity_flip` | 7.44 | Pass |
| `terrain_render` | 7.36 | Pass |
| `xr_lean` | 7.32 | Fail |
| `pickup_pop` | 7.30 | Pass |
| `npc_glance` | 7.27 | Pass |
| `item_split` | 7.26 | Pass |
| `place_glance` | 7.21 | Pass |
| `icon_bake` | 7.13 | Pass |
| `xr_reach` | 7.06 | Pass |
| `tooltips` | 6.98 | Pass |
| `body_views` | 6.93 | Pass |
| `doorway` | 6.79 | Pass |
| `xr_grip` | 6.65 | Pass |
| `gravity_cube` | 6.64 | Pass |
| `exit_game` | 6.54 | Pass |
| `gpu_light_stress` | 6.49 | Pass |
| `xr_play` | 6.45 | Fail |
| `gpu_mesh_views` | 6.43 | Pass |
| `free_roam_pose` | 6.10 | Pass |
| `terrain_meshing_view` | 5.85 | Pass |
| `bite_hitch` | 5.55 | Pass |
| `vox_instances_stress` | 5.43 | Pass |
| `water_block` | 5.32 | Pass |
| `tilemap` | 5.30 | Pass |
| `vox_instance_render` | 5.18 | Pass |
| `block_activate` | 5.15 | Pass |
| `parts` | 5.08 | Pass |
| `quest_tracking` | 4.99 | Pass |
| `place_mesh` | 4.85 | Pass |
| `stream_unload` | 4.66 | Pass |
| `ui_leaks` | 4.56 | Pass |
| `terrain_gpu_mesh` | 4.48 | Pass |
| `item_sizes` | 4.14 | Pass |
| `door_glow` | 4.03 | Pass |
| `furniture_fillers` | 4.03 | Pass |
| `door_save` | 4.02 | Pass |
| `ground_removed` | 3.92 | Pass |
| `block_data_queue` | 3.92 | Pass |
| `chest_weights` | 3.45 | Pass |
| `aura_particles` | 3.18 | Pass |
| `npc_quests` | 3.12 | Pass |
| `skeleton_render` | 3.09 | Pass |
| `instance_views` | 2.97 | Pass |
| `model_views` | 2.96 | Pass |
| `music_theme` | 2.84 | Pass |
| `model_shapes` | 2.78 | Pass |
| `hit_sound_links` | 2.71 | Pass |
| `npc_skeletons` | 2.71 | Pass |
| `water_tile` | 2.69 | Pass |
| `block_frames` | 2.68 | Pass |
| `body_drops` | 2.68 | Pass |
| `npc_speed` | 2.68 | Pass |
| `exit_menu` | 2.57 | Pass |
| `sound_balance` | 2.44 | Pass |
| `flecs_event_scaling` | 2.22 | Pass |
| `music_compose` | 2.06 | Pass |
| `music_timing` | 1.92 | Pass |
| `xr_options` | 1.75 | Pass |
| `xr_sim_eyes` | 1.69 | Pass |
| `terrain_ledges` | 1.65 | Pass |
| `terrain_ledges_x` | 1.64 | Pass |
| `particle_lifetime` | 1.54 | Pass |
| `taskbar_lifecycle` | 1.52 | Pass |
| `sound_trigger` | 1.52 | Pass |
| `terrain_collisions` | 1.39 | Pass |
| `terrain_collisions_x` | 1.36 | Pass |
| `terrain_collisions_up` | 1.36 | Pass |
| `terrain_collisions_z` | 1.35 | Pass |
| `ui_layout` | 1.21 | Pass |
| `lint_systems` | 1.19 | Pass |
| `gpu_restore` | 1.17 | Fail |
| `gravity_cube_model` | 1.16 | Pass |
| `text_order` | 1.16 | Pass |
| `tooltip_blink` | 1.13 | Pass |
| `gpu_mesh` | 1.13 | Pass |
| `options_tabs` | 1.11 | Pass |
| `mouse_click` | 1.11 | Pass |
| `ui_scale` | 1.09 | Pass |
| `terrain_ledges_30fps` | 1.07 | Pass |
| `physics3_gravity` | 1.07 | Pass |
| `physics2_rotation` | 1.07 | Pass |
| `physics3_motion` | 1.07 | Pass |
| `physics3_rotation` | 1.07 | Pass |
| `physics3_rolling_gravity` | 1.07 | Pass |
| `physics2_motion` | 1.07 | Pass |
| `physics3_perf` | 1.05 | Pass |
| `xr_menu` | 1.05 | Pass |
| `sound_instruments` | 1.03 | Pass |
| `physics2_perf` | 1.02 | Pass |
| `lines_spawn` | 1.01 | Pass |
| `terrain_collisions_30fps` | 0.93 | Pass |
| `scroll_wheel` | 0.89 | Pass |
| `load_games` | 0.89 | Pass |
| `window_stack_input` | 0.86 | Pass |
| `list_order` | 0.84 | Pass |
| `sdl_window_mode` | 0.83 | Pass |
| `sdl_window_restore` | 0.81 | Pass |
| `scrollview_overlap` | 0.81 | Pass |
| `font_glyphs_fill` | 0.81 | Pass |
| `physics2_rotation_30fps` | 0.80 | Pass |
| `physics3_rolling_gravity_30fps` | 0.79 | Pass |
| `realm_name_input` | 0.79 | Pass |
| `physics2_motion_30fps` | 0.79 | Pass |
| `physics3_motion_30fps` | 0.79 | Pass |
| `physics3_gravity_30fps` | 0.78 | Pass |
| `physics3_rotation_30fps` | 0.78 | Pass |
| `options_toggle` | 0.77 | Pass |
| `tween_end` | 0.73 | Pass |
| `canvas_resize_input` | 0.73 | Pass |
| `terrain_ledges_10fps` | 0.69 | Pass |
| `eyelids` | 0.67 | Pass |
| `font_glyphs` | 0.67 | Pass |
| `camera_filter` | 0.66 | Fail |
| `sdl_resize` | 0.64 | Pass |
| `terrain_collisions_10fps` | 0.64 | Pass |
| `node_graphs` | 0.63 | Pass |
| `camera_viewport_spawn` | 0.62 | Pass |
| `mouse_leave` | 0.62 | Pass |
| `input_focus_lifecycle` | 0.62 | Pass |
| `realm_info` | 0.61 | Pass |
| `sdl_alt_tab` | 0.61 | Pass |
| `shared_canvas_input` | 0.60 | Pass |
| `settings_apply` | 0.60 | Pass |
| `physics3_motion_10fps` | 0.60 | Pass |
| `input_field` | 0.60 | Pass |
| `physics3_rotation_10fps` | 0.60 | Pass |
| `physics3_gravity_10fps` | 0.59 | Pass |
| `camera_downscale` | 0.59 | Pass |
| `physics2_rotation_10fps` | 0.59 | Pass |
| `physics2_motion_10fps` | 0.59 | Pass |
| `sdl_touch` | 0.59 | Pass |
| `jingles` | 0.58 | Pass |
| `physics3_rolling_gravity_10fps` | 0.58 | Pass |
| `biome_heights` | 0.58 | Pass |
| `sdl_text_input` | 0.57 | Pass |
| `tooltip_gone` | 0.57 | Pass |
| `node_parallel` | 0.56 | Pass |
| `settings_drag` | 0.56 | Pass |
| `mesh_colors_wait` | 0.55 | Pass |
| `node_reentry` | 0.54 | Pass |
| `particle2_lifetime` | 0.54 | Pass |
| `ui3_hidden_row_relabel` | 0.54 | Pass |
| `camera_split_screen` | 0.54 | Pass |
| `sdl_window` | 0.54 | Pass |
| `particle2_motion` | 0.54 | Pass |
| `sdl_quit` | 0.54 | Pass |
| `slider_drag` | 0.54 | Pass |
| `input_field_boundaries` | 0.53 | Pass |
| `sound_formats` | 0.53 | Pass |
| `node_removed_choice` | 0.53 | Pass |
| `lines_render` | 0.53 | Pass |
| `mass_delete` | 0.53 | Pass |
| `settings_load` | 0.53 | Pass |
| `euler_limits` | 0.53 | Pass |
| `transform_hierarchy` | 0.52 | Pass |
| `flecs_event_costs` | 0.52 | Pass |
| `links_cleanup` | 0.51 | Pass |
| `lod_refine_border` | 0.50 | Pass |
| `mass_spawn` | 0.50 | Pass |
| `voice_sounds` | 0.50 | Pass |
| `gpu_mesh_lights` | 0.48 | Pass |
| `gpu_light_views` | 0.48 | Pass |
| `vegetation_cover` | 0.48 | Pass |
| `music_plays` | 0.47 | Pass |
| `furniture_models` | 0.47 | Pass |
| `town_build` | 0.46 | Pass |
| `lod_coarsen` | 0.46 | Pass |
| `wfc` | 0.46 | Pass |
| `gpu_mesh_grow` | 0.46 | Pass |
| `ui3_deferred_spawn` | 0.46 | Pass |
| `links_api` | 0.45 | Pass |
| `octree_flatten` | 0.45 | Pass |
| `octree_basics` | 0.45 | Pass |
| `chunk_io` | 0.45 | Pass |
| `spawn_queue` | 0.45 | Pass |
| `settings_spawn` | 0.44 | Pass |
| `chunk_order` | 0.44 | Pass |
| `vegetation_trees` | 0.44 | Pass |
| `mountain_vegetation` | 0.44 | Pass |
| `physics_steps` | 0.44 | Pass |
| `population` | 0.44 | Pass |
| `gpu_mesh_neighbours` | 0.43 | Pass |
| `model_nodes` | 0.43 | Pass |
| `music_file` | 0.43 | Pass |
| `octree_set_get` | 0.43 | Pass |
| `quest_tracker_text` | 0.43 | Pass |
| `block_vox_light_sampling` | 0.43 | Pass |
| `block_table_remap` | 0.43 | Pass |
| `sdl_mouse_focus` | 0.43 | Pass |
| `stick_deadzone` | 0.43 | Pass |
| `xr_room_scale` | 0.43 | Pass |
| `hashmaps` | 0.42 | Pass |
| `gizmo_owner` | 0.42 | Pass |
| `ik_two_bone` | 0.42 | Pass |
| `hit_sounds` | 0.42 | Pass |
| `octree_links` | 0.42 | Pass |
| `camera_viewports` | 0.42 | Pass |
| `octree_reduce` | 0.42 | Pass |
| `octree_clone_move` | 0.42 | Pass |
| `placement_box` | 0.42 | Pass |
| `transform2_hierarchy` | 0.42 | Pass |
| `vox_mesh_grid` | 0.42 | Pass |
| `camera_projection_angles` | 0.42 | Pass |
| `sound_wav_decode` | 0.41 | Pass |
| `vegetation_across` | 0.41 | Pass |
| `block_datas_io` | 0.41 | Pass |
| `gpu_links` | 0.41 | Pass |
| `transform3_scale_hierarchy` | 0.41 | Pass |
| `chunk_sides` | 0.41 | Pass |
| `particle_burst` | 0.41 | Pass |
| `particle_stream` | 0.41 | Pass |
| `vox_import` | 0.41 | Pass |
| `camera_look_at` | 0.41 | Pass |
| `gpu_mesh_uvs` | 0.41 | Pass |
| `sound_mixer` | 0.41 | Pass |
| `camera_frustum` | 0.41 | Pass |
| `gpu_mesh_materials` | 0.41 | Pass |
| `region_place_prior` | 0.41 | Pass |
| `camera_free_roam` | 0.41 | Pass |
| `camera_projection` | 0.41 | Pass |
| `lod_lights` | 0.41 | Pass |
| `nf_all_deferred` | 0.41 | Pass |
| `nf_prefabs_deferred` | 0.41 | Pass |
| `nf_realm_many` | 0.41 | Pass |
| `node_graph_cleanup` | 0.41 | Pass |
| `octree_debug` | 0.41 | Pass |
| `octree_optimize` | 0.41 | Pass |
| `sound_file_processing` | 0.41 | Pass |
| `text_data` | 0.41 | Pass |
| `transform3_matrices` | 0.41 | Pass |
| `animation_alpha` | 0.41 | Pass |
| `flecs_wildcard_remove` | 0.41 | Pass |
| `nf_realm_full` | 0.41 | Pass |
| `octree_leaks` | 0.41 | Pass |
| `octree_queries` | 0.41 | Pass |
| `particle2_prefab` | 0.41 | Pass |
| `particle_emit` | 0.41 | Pass |
| `region_place` | 0.41 | Pass |
| `smooth_lights` | 0.41 | Pass |
| `sound_voice_lifecycle` | 0.41 | Pass |
| `texture_outline` | 0.41 | Pass |
| `transform2_matrices` | 0.41 | Pass |
| `camera_orthographic` | 0.41 | Pass |
| `colors_hsv` | 0.41 | Pass |
| `font_fill` | 0.41 | Pass |
| `instance_id_reuse` | 0.41 | Pass |
| `maths_bounds` | 0.41 | Pass |
| `maths_matrices` | 0.41 | Pass |
| `nf_cascade_deferred` | 0.41 | Pass |
| `nf_realm_spawn_deferred` | 0.41 | Pass |
| `children` | 0.41 | Pass |
| `dynamic_arrays` | 0.41 | Pass |
| `grass_tufts` | 0.41 | Pass |
| `maths_quaternions` | 0.41 | Pass |
| `nf_mixed_deferred` | 0.41 | Pass |
| `texture_lines` | 0.41 | Pass |
| `animation_shake` | 0.40 | Pass |
| `binary_size` | 0.40 | Pass |
| `octree_neighbors` | 0.40 | Pass |
| `prefab_variants` | 0.40 | Pass |
| `camera_xr_eyes` | 0.40 | Pass |
| `maths_vectors` | 0.40 | Pass |
| `nf_realm` | 0.40 | Pass |
| `nf_cascade` | 0.40 | Pass |
| `texture_noise` | 0.40 | Pass |
| `chunk_positions` | 0.40 | Pass |
| `tween_curves` | 0.40 | Pass |
| `octree_nearby` | 0.40 | Pass |
