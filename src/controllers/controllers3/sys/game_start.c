// Enable Camera Streamer
void enable_camera_streaming(ecs *world, entity player) {
    byte depth = terrain_depth;
    zox_geter_value(player, CameraLink, entity, camera);
    zox_geter_value(camera, Position3D, float3, position);
    zox_geter_value(player, GameLink, entity, game);
    if (!game || !zox_has(game, RealmLink)) {
        return;
    }
    zox_geter_value(game, RealmLink, entity, realm);
    if (!realm || !zox_has(realm, TerrainLink)) {
        return;
    }
    entity terrain = zox_get_value(realm, TerrainLink)
    if (!terrain) {
        return;
    }
    zox_geter_value(terrain, BlockScale, float, terrain_scale);
    int3 terrain_position = real_position_to_chunk_position(position, powers_of_two[depth], terrain_scale);
    zox_set(camera, StreamLink, { terrain });
    zox_set(camera, StreamPoint2, { { terrain_position.x, terrain_position.z }});
    zox_set(camera, StreamDirty2, { zox_dirty_trigger });
    zox_set(camera, StreamPoint, { terrain_position });
    zox_set(camera, StreamDirty, { zox_dirty_trigger });
    if (is_log_streaming) {
        zox_log("+ terrain spawning started at [%f]", zox_current_time);
    }
    // Attach to our terrain's event
    zox_set(terrain, EventInput, { player });
    zox_set(terrain, Loaded, { zox_load_begin });
}

// TODO: Refactor Player UI Spawning to Initialize System

// 1) Waits for Fadeout
// 2) Triggers Camera Streaming
// 3) Spawns Player Character
// 4) Spawns Player UI
zox_sys2(PlayerGame3StartSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(GameLink);
    zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(GameLink, game);
        zox_sys_i(CameraLink, camera);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (state->value != zox_player_state_loading) {
            continue;
        }
        zox_geter_value(game->value, RealmLink, entity, realm);
        zox_geter(realm, SaveGamePath, path);
        float3 position;
        float3 spawn_euler;
        float4 spawn_rotation;
        byte is_new_game = !has_save_game_file(path->value, "player.dat");
        if (is_new_game) {
            position = (float3) { 8, 8.5f, 8 };
            spawn_euler = float3_zero;
            spawn_rotation = quaternion_identity;
        } else {
            // If has save game
            load_character_p(world, realm, e, &position, &spawn_euler, &spawn_rotation);
        }
        // zox_log("Spawning Player Terrain Chunk at [%fx%fx%f] @ [%f]", position.x, position.y, position.z, zox_current_time);
        zox_set(camera->value, Position3D, { position });
        zox_set(camera->value, Euler, { spawn_euler });
        zox_set(camera->value, Rotation3D, { spawn_rotation });
        // Alert our player too
        zox_set(e, PlayerState, { zox_player_state_starting });
        zox_set(e, PlayerStateDirty, { zox_dirty_trigger });
        // waits for fadeout?
        double delay = game_load_player_delay + game_load_fade_transition_time;
        delay_event(world, &enable_camera_streaming, e, delay);
    }
} zox_sys_end(PlayerGame3StartSystem);
