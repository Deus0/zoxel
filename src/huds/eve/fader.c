const double game_spawn_terrain_delay = 0.1;
const double game_spawn_terrain_fade_delay = 1.4;
const double game_load_fade_transition_time = 0.5;

void game_state_fader(ecs* world, entity game, byte state) {
    if (state != zox_game_load_start) {
        return;
    }
    iter it2 = zox_children(world, game);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity player = it2.entities[j];
            if (!zox_has(player, Player)) {
                continue;
            }
            if (!zox_has(player, CanvasLink)) {
                continue;
            }
            entity canvas = zox_getv(player, CanvasLink);
            if (!zox_valid(canvas)) {
                continue;
            }
            if (state == zox_game_load_start) {
                trigger_canvas_fade_in(
                    world,
                    canvas,
                    0,
                    game_load_fade_transition_time);
            }
        }
    }
}

/*zox_sys2(GameStartFaderSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameStateDirty);
    zox_sys_in(GameState);
    zox_sys_in(PlayerLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GameStateDirty, dirty);
        zox_sys_i(GameState, state);
        zox_sys_i(PlayerLinks, players);
        if (!(dirty->value == zox_dirty_active &&
            (state->value == zox_game_load_start ||
            state->value == zox_game_state_play_begin)
        )) {
            continue;
        }
        for (int j = 0; j < players->length; j++) {
            entity player = players->value[j];
            entity canvas = zox_get_link(world, player, Canvas);
            if (state->value == zox_game_load_start) {
                trigger_canvas_fade_in(world, canvas, 0, game_load_fade_transition_time);
            }
        }
    }
} zox_sys_end(GameStartFaderSystem);*/
