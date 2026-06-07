extern entity get_linked_character(ecs*, entity);

// NOTE: Sets to playing when resuming from pause
zox_sys2(PlayerStateSystem) {
    byte dbg_log = 0;
    double pause_time = 0.3;
    double resume_time = 0.3;
    double respawn_time = 16;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(PlayerStateDirty);
    zox_sys_out(PlayerState);
    zox_sys_out(PlayerStateTimer);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(PlayerStateDirty, dirty);
        zox_sys_o(PlayerState, state);
        zox_sys_o(PlayerStateTimer, timer);
        if (timer->value) {
            double time_passed = zox_current_time - timer->value;
            if (state->value == zox_player_state_pause_begin) {
                if (time_passed >= pause_time) {
                    timer->value = 0;
                    state->value = zox_player_state_paused;
                    dirty->value = zox_dirty_trigger;
                    if (dbg_log) {
                        zox_log("Player is now [Paused]");
                    }
                }
            } else if (state->value == zox_player_state_resume_begin) {
                if (time_passed >= resume_time) {
                    timer->value = 0;
                    state->value = zox_player_state_play_begin;
                    dirty->value = zox_dirty_trigger;
                    if (dbg_log) {
                        zox_log("Player is now [Play Begin]");
                    }
                }
            } else if (state->value == zox_player_state_respawning) {
                if (time_passed >= respawn_time) {
                    timer->value = 0;
                    state->value = zox_player_state_respawn;
                    dirty->value = zox_dirty_trigger;
                    if (dbg_log) {
                        zox_log("Player is now [Respawn] from [Respawning]");
                    }
                }
            } else {
                zox_loge("Player State is wrong when timer is on [%i]", state->value);
                timer->value = 0;
            }
            continue;
        }
        // zox_dirty_active
        if (dirty->value != zox_dirty_end) {
            continue;
        }
        // Simple Next States
        if (state->value == zox_player_state_loading) {
            state->value = zox_player_state_starting;
            dirty->value = zox_dirty_trigger;
            if (dbg_log) {
                zox_log("Player is now starting from load.");
            }
        } else if (state->value == zox_player_state_play_begin) {
            state->value = zox_player_state_playing;
            dirty->value = zox_dirty_trigger;
            if (dbg_log) {
                zox_log("Player is now [Playing] from [PlayBegin]")
            }
        } else if (state->value == zox_player_state_respawn_begin) {
            state->value = zox_player_state_respawning;
            dirty->value = zox_dirty_trigger;
            entity game = zox_get_parent(world, e);
            byte game_state = zox_getv(game, GameState);
            if (game_state == zox_game_state_paused) {
                zox_set(game, GameStateTarget, { zox_game_state_respawn_on_pause });
            }
            if (dbg_log) {
                zox_log("Player is now [Respawning] from [RespawnBegin]");
            }
        } else if (state->value == zox_player_state_respawn) {
            state->value = zox_player_state_play_begin;
            dirty->value = zox_dirty_trigger;
            if (dbg_log) {
                zox_log("Player is now [PlayBegin] from [Respawn]");
            }
        }
        // States that exit on Time (Hah!)
        else if (state->value == zox_player_state_pause_begin) {
            timer->value = zox_current_time;
            if (dbg_log) {
                zox_log("Player will soon [Pause]");
            }
        } else if (state->value == zox_player_state_resume_begin) {
            timer->value = zox_current_time;
            if (dbg_log) {
                zox_log("Player will soon [Resume]");
            }
        } else if (state->value == zox_player_state_respawning) {
            timer->value = zox_current_time;
            if (dbg_log) {
                zox_log("Player will soon [Respawn]");
            }
        }
    }
} zox_sys_end(PlayerStateSystem);
