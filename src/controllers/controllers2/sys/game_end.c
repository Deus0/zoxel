void PlayerGame2EndSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_out(CameraLink);
    zox_sys_out(CharacterLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_o(CameraLink, camera);
        zox_sys_o(CharacterLink, character);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (state->value != zox_player_state_main_menu) {
            continue;
        }

        // TODO: Do this in game not in player system
        if (zox_valid(world_grid2D)) {
            zox_delete(world_grid2D);
        }

        zox_delete(character->value);
        character->value = 0;

        camera->value = 0;

        spawn_main_menu(world, e, game_name);
    }
} zoxd_system2(PlayerGame2EndSystem);