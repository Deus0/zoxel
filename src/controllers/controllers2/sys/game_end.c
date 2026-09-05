zox_sys2(PlayerGame2EndSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        if (!(dirty->value == zox_dirty_active && state->value == zox_player_state_main_menu)) {
            continue;
        }
        // TODO: Do this in game not in player system
        if (zox_valid(world_grid2D)) {
            zox_delete(world_grid2D);
        }
        entity character = zox_get_link(world, e, Character);
        zox_delete(character);
        // character->value = 0;
        // camera->value = 0;
        // spawn_main_menu(world, e, game_name);
    }
} zox_sys_end(PlayerGame2EndSystem);
