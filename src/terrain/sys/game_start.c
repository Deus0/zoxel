extern const double game_spawn_terrain_delay;

void game_state_terrain(ecs* world, entity game, byte state) {
    if (state != zox_game_terrain_spawn) {
        return;
    }
    entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm)) {
        zox_loge("[game_state_terrain] Invalid [realm]");
        return;
    }
    /*entity realm = zox_get_child_by_id(
        world,
        game,
        zox_id(Realm));*/
    /*if (!zox_valid(realm)) {
        zox_loge("[game_state_terrain] Invalid [realm]");
        return;
    }*/
    lint realm_seed = zox_getv(realm, Seed);
    lint terrain_seed = realm_seed;
    // for now just do this
    if (zox_game_type == zox_game_mode_3D) {
        spawn_terrain(
            world,
            realm,
            terrain_seed);
    } else if (zox_game_type == zox_game_mode_2D) {
        spawn_grid2D(world);
    }
}

// NOTE: When starting game, it Spawns the Terrain!
/*zox_sys2(TerrainGameStartSystem) {
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
        lint realm_seed = zox_getv(realm, Seed);
        // for now just do this
        lint terrain_seed = realm_seed;
        if (zox_game_type == zox_game_mode_3D) {
            // zox_log("Game Spawning Terrain @ [%f]", zox_current_time);
            // delay_event(world, &game_start_terrain3D, game, game_spawn_terrain_delay);
            entity terrain = spawn_terrain(
                world,
                realm,
                terrain_seed);
        } else if (zox_game_type == zox_game_mode_2D) {
            // delay_event(world, &game_start_terrain2D, game, game_spawn_terrain_delay);
            // spawn_terrain_on_realm(world, realmLink->value);
            spawn_grid2D(world);
        }
    }
} zox_sys_end(TerrainGameStartSystem);*/
