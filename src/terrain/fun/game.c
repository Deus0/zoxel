// End Game
void game_end_terrain(ecs *world, entity game) {
    if (zox_game_type == zox_game_mode_3D) {
        zox_geter(game, RealmLink, realm);
        zox_geter(realm->value, TerrainLink, terrain);
        if (zox_valid(terrain->value)) {
            zox_delete(terrain->value);
            zox_set(realm->value, TerrainLink, { 0 });
        }
        local_terrain = 0;
    }
}

void game_start_terrain2D(ecs *world, entity game) {
    (void) game;
    spawn_grid2D(world);
}

void spawn_terrain_on_realm(ecs *world, entity realm) {
    entity terrain = spawn_terrain_streaming(
        world,
        realm,
        prefab_terrain
    );
    zox_set(terrain, RealmLink, { realm });
    zox_set(realm, TerrainLink, { terrain }); // link terrain to realm too
    local_terrain = terrain;
}

void game_start_terrain3D(ecs *world, entity game) {
    zox_geter(game, RealmLink, realmLink)
    spawn_terrain_on_realm(world, realmLink->value);
}

// Start Game
extern const double game_spawn_terrain_delay;
void game_start_terrain(ecs *world, entity game) {
    if (zox_game_type == zox_game_mode_3D) {
        delay_event(world, &game_start_terrain3D, game, game_spawn_terrain_delay);
    } else if (zox_game_type == zox_game_mode_2D) {
        delay_event(world, &game_start_terrain2D, game, game_spawn_terrain_delay);
    }
    unlock_achievement("test_achievement"); // idk if this can be per player
}


// Entry Point
// state change goes to start and end functions
void game_state_terrain(ecs *world, entity game, byte old_game_state, byte state) {
    if (state == zox_game_playing_start) {
        game_start_terrain(world, game);
    } else if (state == zox_game_start) {
        if (is_end_game_delays) {
            delay_event(world, &game_end_terrain, game, end_game_delay2);
        } else {
            game_end_terrain(world, game);
        }
    }
}