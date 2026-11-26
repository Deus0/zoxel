// the game's state changed (its core Dirty): at boot, the editor's windows (no main menu)
void game_state_zedit(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GameState, state);
        if (state->value == zox_game_boot) {
            const entity player = zox_players[0];
            const entity canvas = zox_get_link(world, player, CanvasLink);
            if (zox_valid(canvas)) {
                ensure_editor_menubar(world, canvas);
                spawn_inspector(world, canvas, player, canvas);
                spawn_window_hierarchy(world, canvas, player, 0);   // every root
                spawn_window_files(world, canvas, player, NULL);
            }
        }
    }
}

void zox_events_zedit(ecs* world) {
    zox_on_add(game_state_zedit, [in] games.GameState, [none] games.Game, [none] core.Dirty);
}
