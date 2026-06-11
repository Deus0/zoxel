extern const double game_spawn_terrain_delay;

// NOTE: When starting game, it Spawns the Terrain!
zox_sys2(TerrainGameStartSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RealmLink);
    zox_sys_in(GameState);
    zox_sys_in(GameStateDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RealmLink, realm);
        zox_sys_i(GameState, state);
        zox_sys_i(GameStateDirty, dirty);
        if (!(state->value == zox_game_state_play_begin && dirty->value == zox_dirty_active)) {
            continue;
        }
        lint realm_seed = zox_getv(realm->value, Seed);
        // for now just do this
        lint terrain_seed = realm_seed;
        if (zox_game_type == zox_game_mode_3D) {
            // zox_log("Game Spawning Terrain @ [%f]", zox_current_time);
            // delay_event(world, &game_start_terrain3D, game, game_spawn_terrain_delay);
            entity terrain = spawn_terrain(world, prefab_terrain, realm->value, terrain_seed);
            local_terrain = terrain;
        } else if (zox_game_type == zox_game_mode_2D) {
            // delay_event(world, &game_start_terrain2D, game, game_spawn_terrain_delay);
            // spawn_terrain_on_realm(world, realmLink->value);
            spawn_grid2D(world);
        }
    }
} zox_sys_end(TerrainGameStartSystem);
