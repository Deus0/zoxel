// TODO: Fade Out + Fade In based on game states
// loading resources
const double game_spawn_terrain_delay = 0.1;
const double game_load_player_delay = 0.2; // 1.1;
// fading
const double game_load_fade_transition_time = 0.5;
// const double game_load_fade_time = 3;

void GameStartFaderSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameStateDirty);
    zox_sys_in(GameState);
    zox_sys_in(PlayerLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GameStateDirty, dirty);
        zox_sys_i(GameState, state);
        zox_sys_i(PlayerLinks, players);

        if (!(dirty->value == zox_dirty_active &&
            (state->value == zox_game_load_start ||
            state->value == zox_game_playing_start)
        )) {
            continue;
        }

        for (int j = 0; j < players->length; j++) {
            entity player = players->value[j];
            zox_geter_value(player, CanvasLink, entity, canvas);

            if (state->value == zox_game_load_start) {
                trigger_canvas_fade_in(
                    world,
                    canvas,
                    0,
                    game_load_fade_transition_time
                );
            } else if (state->value == zox_game_playing_start) {
                trigger_canvas_fade_out(
                    world,
                    canvas,
                    game_spawn_terrain_delay + 0.1,
                    game_load_fade_transition_time
                );
            }
            /*trigger_canvas_fade_transition(
                world,
                canvas,
                game_load_fade_transition_time,
                game_load_fade_time
            );*/
        }
        // set animation fadeout event to change state
    }
} zoxd_system2(GameStartFaderSystem);