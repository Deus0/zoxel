zox_sys2(PlayerScreenFadeSystem) {
    // const double game_spawn_terrain_delay = 0.1;
    // double game_load_player_delay = 0.2; // 1.1;
    double game_load_fade_transition_time = 0.5;
    double game_spawn_terrain_fade_delay = 1.4;
    // byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(CanvasLink);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(CanvasLink, canvas);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (state->value == zox_player_state_starting) {
            trigger_canvas_fade_out(world, canvas->value, game_spawn_terrain_fade_delay, game_load_fade_transition_time);
        }
    }
} zox_sys_end(PlayerScreenFadeSystem);

