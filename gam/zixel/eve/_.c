// the game's state changed (its core Dirty): at boot, the start menu
void game_state_zixel(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GameState, state);
        if (state->value == zox_game_boot) {
            spawn_boot_menu(world);
        }
    }
}

void zox_events_zixel(ecs* world) {
    zox_on_add(game_state_zixel, [in] games.GameState, [none] games.Game, [none] core.Dirty);
}
