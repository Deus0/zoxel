extern entity local_terrain;

zox_sys2(PlayerGameStateSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameStateDirty);
    zox_sys_in(GameState);
    zox_sys_in(LastGameState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GameStateDirty, dirty);
        zox_sys_i(GameState, state);
        zox_sys_i(LastGameState, last);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity players[zox_children_capacity];
        uint players_length = zox_get_children_by_id(
            world,
            e,
            players,
            zox_children_capacity,
            zox_id(Player));
        for (uint j = 0; j < players_length; j++) {
            entity e2 = players[j];
            if (state->value == zox_game_state_the_end) {
                if (dbg_log) {
                    zox_log("Game Setting player [%s] to [The End]", zox_getn(e2));
                }
                zox_setv(e2, PlayerState, zox_player_state_the_end);
            } else if (state->value == zox_game_terrain_spawn) {
                if (dbg_log) {
                    zox_log("Game Setting player [%s] to Loading", zox_get_name(e2));
                }
                zox_setv(e2, PlayerState, zox_player_state_loading);
            } else if (state->value == zox_game_start) {
                zox_setv(e2, PlayerState, zox_player_state_main_menu_begin);
                if (dbg_log) {
                    zox_log("Game Setting player [%s] to [MainMenu]", zox_get_name(e2));
                }
            } else if (state->value == zox_game_state_paused) {
                if (dbg_log) {
                    zox_log("Game Setting player [%s] to Paused", zox_get_name(e2));
                }
                zox_setv(e2, PlayerState, zox_player_state_pause_begin);
            } else if (last->value == zox_game_state_paused && state->value == zox_game_state_playing) {
                if (dbg_log) {
                    zox_log("Game Setting player [%s] to Resume", zox_get_name(e2));
                }
                zox_setv(e2, PlayerState, zox_player_state_resume_begin);
            } else {
                continue;
            }
            zox_setv(e2, PlayerStateDirty, zox_dirty_trigger);
        }
    }
} zox_sys_end(PlayerGameStateSystem);
