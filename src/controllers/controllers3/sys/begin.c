TerrainPlace find_position_in_terrain(ecs *world, entity terrain) {
    zox_geter(terrain, ChunkLinks, chunks);
    entity chunk;
    int3 cposition = int3_zero;
    const VoxelNode *voxel_node_above = NULL;
    byte node_depth = 0;
    byte found_position = 0;
    byte3 in_chunk_position = byte3_zero;
    for (int i = render_distance_y; i >= -render_distance_y; i--) {
        cposition.y = i;
        chunk = int3_hashmap_get(chunks->value, cposition);
        if (!zox_valid(chunk)) {
            zox_logw("Chunk Missing at [0x%ix0]", i);
            continue;
        }
        zox_geter(chunk, VoxelNode, chunkd);
        node_depth = zox_get_value(chunk, NodeDepth);
        if (find_random_position_on_ground(chunkd, voxel_node_above, node_depth, 200, &in_chunk_position)) {
            // zox_log("Found Position for Player [%i] of [%i] at [%ix%ix%i]", i, render_distance_y, in_chunk_position.x, in_chunk_position.y, in_chunk_position.z);
            found_position = 1;
            break;
        }
        voxel_node_above = chunkd;
    }
    if (!found_position) {
        int count = count_int3_hashmap(chunks->value);
        zox_loge("Failed finding spawn position for player; Chunks [%i]", count);
        if (!count) {
            zox_loge(" -> ? No chunks!");
        }
    }
    zox_geter_value(terrain, BlockScale, float, tscale);
    float3 positionf = byte3_to_float3(in_chunk_position);
    float3_scale_p(&positionf, tscale);
    if (zox_valid(chunk)) {
        zox_geter_value(chunk, Position3D, float3, chunk_positionf);
        float3_add_float3_p(&positionf, chunk_positionf); // chunk
    }
    float3_add_float3_p(&positionf, float3_single(tscale * 0.5f));
    float4 rotation = quaternion_from_euler( (float3) { 0, (rand() % 361) * degreesToRadians, 0 });
    return (TerrainPlace) {
        .chunk = chunk,
        .chunk_position = cposition,
        .position = positionf,
        .rotation = rotation,
    };
}

entity game_start_player_new(ecs *world, entity player, float3* spawned_position) {
    entity realm;
    entity terrain;
    entity camera;
    if (!get_player_linked_things(world, player, &realm, &terrain, &camera)) {
        *spawned_position = float3_zero;
        return 0;
    }
    TerrainPlace placer = find_position_in_terrain(world, terrain);
    *spawned_position = placer.position;
    byte render_depth = 5;
    entity e = spawn_character3_player(world, prefab_character3_player, realm, terrain, 0, render_depth, 0, placer.position, quaternion_identity, "Bob", player);
    return e;
}

entity game_start_player_load(ecs *world, entity player, float3* spawned_position) {
    entity realm;
    entity terrain;
    entity camera;
    if (!get_player_linked_things(world, player, &realm, &terrain, &camera)) {
        *spawned_position = float3_zero;
        return 0;
    }
    // entity model = string_hashmap_get(files_hashmap_voxes, new_string_data(player_vox_model));
    zox_geter_value(terrain, BlockScale, float, terrain_scale);
    zox_geter(terrain, ChunkLinks, chunks);
    TerrainPlace placer;
    placer.chunk = 0;
    // load position for spawning
    load_character_p(world, realm, player, &placer.position, &placer.euler, &placer.rotation);
    byte depth = terrain_depth;
    int3 cposition = real_position_to_chunk_position(placer.position, powers_of_two[depth], terrain_scale);
    placer.chunk = int3_hashmap_get(chunks->value, cposition);
    *spawned_position = placer.position; // load_character_transform(world, realm, e);
    byte render_depth = 5;
    entity e = spawn_character3_player(world, prefab_character3_player, realm, terrain, 0, render_depth, 0, placer.position, quaternion_identity, "Bob", player);
    return e;
}

// NOTE: Player Spawns Player Character
zox_sys2(PlayerBeginSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameLink);
    zox_sys_in(CharacterLink);
    zox_sys_out(PlayerState);
    zox_sys_out(PlayerStateDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GameLink, game);
        zox_sys_i(CharacterLink, character);
        zox_sys_o(PlayerState, state);
        zox_sys_o(PlayerStateDirty, dirty);
        // Now Spawning Character
        if (!(state->value == zox_player_state_starting && dirty->value == zox_dirty_active)) {
            continue;
        }
        if (!zox_valid(game->value)) {
            zox_loge("No Game on Player.");
            continue;
        }
        // Checks if terrain is done loading
        zox_geter_value(game->value, RealmLink, entity, realm);
        if (!zox_valid(realm)) {
            zox_loge("No Realm on Game");
            continue;
        }
        if (!zox_has(realm, TerrainLink)) {
            zox_loge("Realm [%s] has no Terrain Link", zox_get_name(realm));
            continue;
        }
        zox_geter_value(realm, TerrainLink, entity, terrain);
        if (!zox_valid(terrain)) {
            zox_loge("Invalid Terrain on Realm");
            continue;
        }
        // Wait for Terrain to load
        byte loaded = zox_getv(terrain, Loaded);
        if (loaded != zox_load_done) {
            if (dbg_log) {
                zox_log("Terrain is still Loading [%i]", loaded);
            }
            // Keep active
            dirty->value = zox_dirty_trigger;
            // zox_set(e, PlayerStateDirty, { zox_dirty_trigger });
            continue;
        }
        if (zox_valid(character->value)) {
            zox_log("Trying to load character twice [zox_player_state_starting]");
            state->value = zox_player_state_play_begin;
            dirty->value = zox_dirty_trigger;
            continue;
        }
        // actually we need to do this on loaded player model for bounds
        // if character
        zox_geter(realm, SaveGamePath, path);
        byte is_new_game = !has_save_game_file(path->value, "player.dat");
        float3 spawn_position;
        if (!is_new_game) {
            game_start_player_load(world, e, &spawn_position);
        } else {
            game_start_player_new(world, e, &spawn_position);
        }
        if (dbg_log) {
            zox_log("Player Character Spawned at [%fx%fx%f]", spawn_position.x, spawn_position.y, spawn_position.z);
        }
        spawn_arrow3D(world, spawn_position, (float3) { 0, 1, 0}, 0.2f, 6, 30);
        // Needs ui spawn after frame
        play_playlist(world, realm, 1);
        // delay_event(world, &spawn_player_game_ui, e, 1);
        if (local_mouse) {
            zox_set(local_mouse, MouseLock, { 1 });
        }
        state->value = zox_player_state_play_begin;
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(PlayerBeginSystem);
