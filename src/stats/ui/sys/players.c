// NOTE: When game pauses we destroy the stats panel
zox_sys2(StatbarsDestroySystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(CanvasLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(CanvasLink, canvas);
        if (dirty->value != zox_dirty_active || state->value != zox_player_state_pause_begin) {
            continue;
        }
        if (!zox_valid(canvas->value)) {
            zox_logw("Canvas is missing from Player [PlayerUIGamePauseSystem]");
            continue;
        }
        entity statbars = zox_get_child_by_id(world, canvas->value, zox_id(StatBars));
        if (zox_valid(statbars)) {
            zox_delete(statbars);
        }
    }
} zox_sys_end(StatbarsDestroySystem);

// NOTE: When Game Starts or Resumes we spawn the stats panel
zox_sys2(StatbarsSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(CanvasLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(CanvasLink, canvas);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (state->value != zox_player_state_play_begin) {
            continue;
        }
        if (!zox_valid(canvas->value)) {
            zox_logw("Canvas is missing from Player [PlayerUIGamePauseSystem]");
            continue;
        }
        entity statbars = zox_get_child_by_id(world, canvas->value, zox_id(StatBars));
        if (!zox_valid(statbars)) {
            zox_log("Spawning Stats Panel ?");
            spawn_menu_game_stats(world, e);
        }
    }
} zox_sys_end(StatbarsSpawnSystem);
