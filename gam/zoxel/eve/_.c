// we should just add dirty hook to the settings entities

void app_initialize_zoxel(ecs* world, entity app) {
    entity e2 = spawn_test_vox_at(
        world,
        float3_zero,
        quaternion_identity,
        (float2) { 1.6f, 2.6f });
    zox_set_parent(world, e2, app);
}

// the game's state changed (its core Dirty): at boot, the start menu
void game_state_zoxel(iter* it) {
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

void zox_events_zoxel(ecs* world) {
    zox_on_add(game_state_zoxel, [in] games.GameState, [none] games.Game, [none] core.Dirty);
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, app_initialize_zoxel);
    zox_on_add(town_music_entered, [none] towns.InTown, [none] players.PlayerCharacter);
    zox_on_add(town_music_left, [none] towns.TownLeft, [none] players.PlayerCharacter);
}
