void game_start_terrain2D(
    ecs *world,
    const entity game
) {
    (void) game;
    spawn_grid2D(world);
}

void spawn_terrain_on_realm(
    ecs *world,
    const entity realm
) {
    // const int3 render_size = (int3) { terrain_spawn_distance, terrain_vertical, terrain_spawn_distance };
    const entity terrain = spawn_terrain_streaming(
        world,
        realm,
        prefab_terrain
    );
    zox_set(terrain, RealmLink, { realm })
    zox_set(realm, TerrainLink, { terrain }) // link terrain to realm too
    local_terrain = terrain;
}

void game_start_terrain3D(
    ecs *world,
    const entity game
) {
    zox_geter(game, RealmLink, realmLink)
    spawn_terrain_on_realm(world, realmLink->value);
}

// Start Game
void game_start_terrain(
    ecs *world,
    const entity game
) {
    if (zox_game_type == zox_game_mode_3D) {
        delay_event(world, &game_start_terrain3D, game, start_game_delay_terrain);
    } else if (zox_game_type == zox_game_mode_2D) {
        delay_event(world, &game_start_terrain2D, game, start_game_delay_terrain);
    }
    unlock_achievement("test_achievement"); // idk if this can be per player
}
