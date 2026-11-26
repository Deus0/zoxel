// the 2D player's state changed (its core Dirty)
void player_game2_end_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        if (!(state->value == zox_player_state_main_menu)) {
            continue;
        }
        // TODO: Do this in game not in player system
        if (zox_valid(world_grid2D)) {
            zox_delete(world_grid2D);
        }
        entity character = zox_get_link(world, e, CharacterLink);
        zox_delete(character);
        // character->value = 0;
        // camera->value = 0;
        // spawn_main_menu(world, e, game_name);
    }
}
