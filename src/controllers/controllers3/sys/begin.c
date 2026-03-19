TerrainPlace find_position_in_terrain(ecs *world, entity terrain) {
    // test bounds for spawning
    float3 bounds = (float3) { 0.5f, 1.0, 0.5f };

    zox_geter(terrain, ChunkLinks, chunks);

    entity chunk;
    int3 cposition = int3_zero;
    byte3 local_position = byte3_zero;
    const VoxelNode *voxel_node_above = NULL;
    byte node_depth = 0;
    byte found_position = 0;

    for (int i = render_distance_y; i >= -render_distance_y; i--) {
        cposition.y = i;
        chunk = int3_hashmap_get(chunks->value, cposition);

        if (!zox_valid(chunk)) {
            zox_logw("Chunk Missing at [0x%ix0]", i);
            continue;
        }

        zox_geter(chunk, VoxelNode, chunkd);
        node_depth = zox_get_value(chunk, NodeDepth);

        local_position = find_random_position_on_ground(
            chunkd,
            voxel_node_above,
            node_depth,
            200);

        if (!byte3_equals(byte3_full, local_position)) {
            found_position = 1;
            break;
        }

        voxel_node_above = chunkd;
    }

    if (!found_position) {
        int count = count_int3_hashmap(chunks->value);
        zox_log_error("Failed finding spawn position for player; Chunks [%i]", count);
        if (!count) {
            zox_log_error(" -> ? No chunks!");
        }
    }

    int vlength = powers_of_two[node_depth];
    int3 chunk_voxel_position = get_chunk_positionv(cposition, int3_single(vlength));

    float3 spawn_position = local_to_real_position_character(local_position,  chunk_voxel_position, bounds, 1);

    // zox_log("Terrain Place Found [%fx%fx%f]", spawn_position.x, spawn_position.y, spawn_position.z)

    return (TerrainPlace) {
        .chunk = chunk,
        .chunk_position = cposition,
        .position = spawn_position,
        .rotation = quaternion_identity,
    };
}

entity spawn_first_chunk(ecs* world, entity realm, entity terrain, float3 position) {

    zox_geter_value(terrain, NodeDepth, byte, tdepth);
    zox_geter_value(terrain, BlockScale, float, terrain_scale);

    int3 cposition = real_position_to_chunk_position(position, powers_of_two[tdepth], terrain_scale);

    entity c;
    zox_mut_begin(terrain, ChunkLinks, chunks3);
    zox_mut_begin(terrain, Chunk2Links, chunks2);

    // TODO: Move chunk spawn code into stream system and out of Controllers

    // Spawn our Tunk for terrain generation
    int2 tposition = (int2) { cposition.x, cposition.z };
    if (!int2_hashmap_has(chunks2->value, tposition)) {
        entity t = spawn_tunk(world, prefab_tunk2, terrain, tposition, 0);
        if (zox_valid(t)) {
            int2_hashmap_add(chunks2->value, tposition, t);
            zox_mut_end(terrain, Chunk2Links);
        }
    }

    if (!int3_hashmap_has(chunks3->value, cposition)) {
        c = spawn_chunk_terrain(world, prefab_chunk_terrain, terrain, cposition, cposition, tdepth, terrain_scale);
        if (zox_valid(c)) {
            int3_hashmap_add(chunks3->value, cposition, c);
            zox_mut_end(terrain, ChunkLinks);
        } else {
            zox_log_error("Failed to spawn chunk [%ix%ix%i]:%lu", cposition.x, cposition.y, cposition.z, c);
        }
    } else {
        c = int3_hashmap_get(chunks3->value, cposition);
    }

    return c;
}

entity game_start_player_new(ecs *world, entity player) {
    entity realm;
    entity terrain;
    entity camera;
    if (!get_player_linked_things(world, player, &realm, &terrain, &camera)) {
        return 0;
    }

    entity model = string_hashmap_get(files_hashmap_voxes, new_string_data(player_vox_model));
    if (!model) {
        zox_log_error("File Not Found [%s]", player_vox_model);
    }

    TerrainPlace placer = find_position_in_terrain(world, terrain);

    spawn_character3D_data spawn_data = {
        .player = player,
        // .model = model,
        .realm = realm,
        .terrain = terrain,
        .terrain_chunk = placer.chunk,
        .chunk_position = placer.chunk_position,
        .position = placer.position, // fake_spawn_position,
        .rotation = quaternion_identity, // placer.rotation
    };

    entity e = spawn_character3_player(world, spawn_data);
    // Add spawned to chunk
    if (zox_valid(placer.chunk)) {
        zox_mut_begin(placer.chunk, ChunkEntities, entities);
        if (add_to_ChunkEntities(entities, e)) {
            zox_mut_end(placer.chunk, ChunkEntities);
        }
        // zox_set(e, DisableGravity, { 0 });
        // zox_set(e, DisableMovement, { 0 });
    } else {
        zox_log_error("Issue with place chunk");
    }

    spawn_arrow3D(world, spawn_data.position, (float3) { 0, 4, 0}, 0.5f, 6, 60);

    // Needs ui spawn after frame
    delay_event(world, &spawn_player_game_ui, player, 0.01);

    return e;
}

entity game_start_player_load(ecs *world, entity player) {
    entity realm;
    entity terrain;
    entity camera;
    if (!get_player_linked_things(world, player, &realm, &terrain, &camera)) {
        return 0;
    }

    entity model = string_hashmap_get(files_hashmap_voxes, new_string_data(player_vox_model));
    zox_geter_value(terrain, BlockScale, float, terrain_scale);
    zox_mut_begin(terrain, ChunkLinks, chunks);
    // zox_mut_begin(terrain, Chunk2Links, chunks2);

    TerrainPlace placer;
    placer.chunk = 0;
    // byte spawned_first_chunk = 0;
    if (!model) {
        zox_log_error("File [%s] Not Found.", player_vox_model);
    }

    // load position for spawning
    load_character_p(world, realm, player, &placer.position, &placer.euler, &placer.rotation);
    byte depth = terrain_depth;
    int3 cposition = real_position_to_chunk_position(placer.position, powers_of_two[depth], terrain_scale);
    placer.chunk = int3_hashmap_get(chunks->value, cposition);

    spawn_character3D_data spawn_data = {
        .player = player,
        // .model = model,
        .realm = realm,
        .terrain = terrain,
        .terrain_chunk = placer.chunk,
        .chunk_position = cposition,
        .position = placer.position,
        .rotation = placer.rotation,
        .euler = placer.euler,
    };

    entity e = spawn_character3_player(world, spawn_data);
    // Add spawned to chunk
    // TODO: This in a system? Character Initialize?
    if (zox_valid(placer.chunk)) {
        zox_mut_begin(placer.chunk, ChunkEntities, entityLinks)
        if (add_to_ChunkEntities(entityLinks, e)) {
            zox_mut_end(placer.chunk, ChunkEntities);
        }
        // zox_set(e, DisableGravity, { 0 });
        // zox_set(e, DisableMovement, { 0 });
    }

    spawn_arrow3D(world, spawn_data.position, (float3) { 0, 4, 0}, 0.5f, 6, 60);

    load_player_e(world, player);
    // load_camera_e(world, realm, camera);

    // TODO: LoadCharacterSystem - States
    // TODO: Remove use of time, and make a LoadedCharacter Event Hook for this!
    // delay_event(world, &load_player_e, player, 0.02);

    // delay_event(world, &spawn_player_game_ui, player, 0.05);
    // Needs ui spawn after frame
    // spawn_player_game_ui(world, player);
    delay_event(world, &spawn_player_game_ui, player, 0.5);

    return e;
}

// event called when terrain spawned
void on_spawned_terrain(ecs *world, entity player) {

    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(player, CameraLink, entity, camera);
    zox_geter_value(game, RealmLink, entity, realm);

    play_playlist(world, realm, 1);
    // actually we need to do this on loaded player model for bounds

    // if character
    if (game_rule_attach_to_character) {

        zox_geter(realm, SaveGamePath, path);
        byte is_new_game = !has_save_game_file(path->value, "player.dat");

        if (!is_new_game) {
            game_start_player_load(world, player);
        } else {
            game_start_player_new(world, player);
        }

    } else {
        set_camera_free(world, camera);
    }
}

zox_sys2(PlayerBeginSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameLink);
    zox_sys_out(PlayerState);
    zox_sys_out(PlayerStateDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GameLink, game);
        zox_sys_o(PlayerState, state);
        zox_sys_o(PlayerStateDirty, dirty);

        if (state->value != zox_player_state_starting) {
            // zox_log(" Waiting [%i]", state->value);
            continue;
        }

        // Checks if terrain is done loading
        zox_geter_value(game->value, RealmLink, entity, realm);
        if (!zox_valid(realm)) {
            zox_log_error("Game has no realm");
            continue;
        }
        if (!zox_has(realm, TerrainLink)) {
            zox_log_error("Realm [%s] has no Terrain Link", zox_get_name(realm));
            continue;
        }

        zox_geter_value(realm, TerrainLink, entity, terrain);
        if (!zox_valid(terrain)) {
            zox_log_error("Invalid Terrain on Realm");
            continue;
        }

        byte loaded = zox_gett_value(terrain, Loaded);
        if (loaded != zox_load_done) {
            // zox_log("   - Terrain Loaded [%i]", loaded);
            continue;
        }

        zox_log("Spawning Player Character @ [%f]", zox_current_time);

        // spawn character here
        zox_sys_e();
        on_spawned_terrain(world, e);

        state->value = zox_player_state_playing;
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(PlayerBeginSystem);
