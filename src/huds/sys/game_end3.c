void main_menu_event_delay(ecs *world, entity e) {
    spawn_main_menu(world, e, game_name);
}

void PlayerUIGame3EndSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(GameLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(GameLink, game);
        if (dirty->value != zox_dirty_active || state->value != zox_player_state_main_menu) {
            continue;
        }
        zox_geter_value(game->value, RealmLink, entity, realm);
        // TODO: Realm is missing at this point, need a main menu realm
        // music - attach to game from music module
        play_playlist(world, realm, 0);
        // spawn main menu here
        delay_event(world, &main_menu_event_delay, e, 2);
        // spawn_main_menu(world, e, game_name);
    }
} zoxd_system2(PlayerUIGame3EndSystem);
