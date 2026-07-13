// TODO: Refactor Player UI Spawning to Initialize System
// 1) Waits for Fadeout
// 2) Triggers Camera Streaming
// 3) Spawns Player Character
// 4) Spawns Player UI
// NOTE: Sets the Game Camera and starts streaming terrain
zox_sys2(GameStartStreamerSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CameraLink);
    zox_sys_out(PlayerState);
    zox_sys_out(PlayerStateDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CameraLink, camera);
        zox_sys_o(PlayerState, state);
        zox_sys_o(PlayerStateDirty, dirty);
        if (!(dirty->value == zox_dirty_active && state->value == zox_player_state_loading)) {
            continue;
        }
        entity game = zox_get_parent(world, e);
        if (!zox_valid(game)) {
            zox_loge("Player [%s] has invalid Game.", zox_get_name(e));
            continue;
        }
        entity realm = zox_getv(game, RealmLink);
        if (!zox_valid(realm) || !zox_has(realm, FolderPath) || !zox_has(realm, TerrainLink)) {
            zox_loge("Game [%s] has invalid Realm.", zox_get_name(game));
            continue;
        }
        entity terrain = zox_getv(realm, TerrainLink);
        if (!zox_valid(terrain)) {
            continue;
        }
        if (!zox_valid(camera->value)) {
            zox_loge("Player has no Camera");
            continue;
        }
        zox_geter(realm, FolderPath, path);
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        byte terrain_chunk_length = powers_of_two[terrain_depth];
        zox_geter_value(terrain, BlockScale, float, terrain_scale);
        float3 position;
        float3 spawn_euler;
        float4 spawn_rotation;
        byte is_new_game = !has_save_game_file(path->value, "player.dat");
        if (is_new_game) {
            // NOTE: Loads first regions in terrain
            position = (float3) { 8, 8.5f, 8 };
            spawn_euler = float3_zero;
            spawn_rotation = quaternion_identity;
            state->value = zox_player_state_new;
            dirty->value = zox_dirty_trigger;
            // Regions / Towns only
            zox_set(camera->value, StreamerLevel, { 0 });
        } else {
            // If has save game
            load_character_player(world, realm, e, &position, &spawn_euler, &spawn_rotation);
            int chunk_position_y = real_position_to_chunk_position1(position.y, terrain_chunk_length, terrain_scale);
            if (!(chunk_position_y >= -render_distance_y && chunk_position_y <= render_distance_y)) {
                zox_logw("Player was out of Terrain Bounds on load");
                position.y = chunk_position_to_real_position1((render_distance_y - 1), terrain_chunk_length, terrain_scale);
            }
            zox_set(camera->value, StreamerLevel, { 1 });
        }
        {
            // Attach to our terrain's event
            zox_set(terrain, EventInput, { e });
            zox_set(terrain, Loaded, { zox_load_begin });
        }
        {
            int3 terrain_position = real_position_to_chunk_position(position, terrain_chunk_length, terrain_scale);
            int2 terrain_position2 = (int2) { terrain_position.x, terrain_position.z };
            entity e2 = camera->value;
            zox_set(e2, Position3D, { position });
            zox_set(e2, Euler, { spawn_euler });
            zox_set(e2, Rotation3D, { spawn_rotation });
            zox_set(e2, StreamLink, { terrain });
            zox_set(e2, StreamPosition, { terrain_position });
            zox_set(e2, StreamPosition2, { terrain_position2 });
            zox_set(e2, StreamDirty, { zox_dirty_trigger });
            zox_set(e2, StreamDirty2, { zox_dirty_trigger });
        }
        if (is_log_streaming) {
            zox_log("+ terrain spawning started at [%f]", zox_current_time);
        }
    }
} zox_sys_end(GameStartStreamerSystem);
