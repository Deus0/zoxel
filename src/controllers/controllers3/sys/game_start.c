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
        zox_log_error("Failed finding spawn position for player");
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

// returns success
byte get_player_linked_things(ecs* world, entity player, entity* realm_out, entity* terrain_out, entity* camera_out) {

    zox_geter_value(player, GameLink, entity, game);
    if (!zox_valid(game)) {
        zox_log_error("Invalid [game]");
        return 0;
    }

    zox_geter_value(game, RealmLink, entity, realm);
    if (!zox_valid(realm) || !zox_has(realm, TerrainLink)) {
        if (!zox_valid(realm)) {
            zox_log_error("Invalid [realm]");
        } else {
            zox_log_error("[realm] has No TerrainLink");
        }
        return 0;
    }

    zox_geter_value(realm, TerrainLink, entity, terrain);
    if (!zox_valid(terrain)) {
        zox_log_error("Invalid [terrain]");
        return 0;
    }

    zox_geter_value(player, CameraLink, entity, camera);
    if (!zox_valid(camera)) {
        zox_log_error("Invalid [camera]");
        return 0;
    }

    *realm_out = realm;
    *terrain_out = terrain;
    *camera_out = camera;
    return 1;
}

entity spawn_first_chunk(ecs* world, entity realm, entity terrain) {

    zox_geter_value(terrain, NodeDepth, byte, tdepth);
    zox_geter_value(terrain, BlockScale, float, terrain_scale);

    float3 position = load_player_position(world, realm);
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
    // float3 fake_spawn_position = (float3) { 4, 4, 4 };

    spawn_character3D_data spawn_data = {
        .player = player,
        .model = model,
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
        zox_set(e, DisableGravity, { 0 });
        zox_set(e, DisableMovement, { 0 });
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
    zox_mut_begin(terrain, Chunk2Links, chunks2);

    TerrainPlace placer;
    placer.chunk = 0;
    byte spawned_first_chunk = 0;
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
        .model = model,
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
        zox_set(e, DisableGravity, { 0 });
        zox_set(e, DisableMovement, { 0 });
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
    delay_event(world, &spawn_player_game_ui, player, 0.01);

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

// Trigger our Streamer to Begin
void delayed_start_streamer(ecs *world, entity player) {

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
    int3 terrain_position = real_position_to_chunk_position(
        position,
        powers_of_two[depth],
        terrain_scale
    );

    zox_set(camera, StreamPoint, { terrain_position });
    zox_set(camera, StreamPoint2, { { terrain_position.x, terrain_position.z }});
    zox_set(camera, StreamLink, { terrain });
    zox_set(camera, StreamDirty, { zox_dirty_trigger });
    zox_set(camera, StreamDirty2, { zox_dirty_trigger });

    if (is_log_streaming) {
        zox_log("+ terrain spawning started at [%f]", zox_current_time);
    }

    zox_set(player, PlayerState, { zox_player_state_starting });
    zox_set(player, PlayerStateDirty, { zox_dirty_trigger });

    spawn_first_chunk(world, realm, terrain);

    // On End Event
    zox_set(terrain, EventInput, { player });
    zox_set(terrain, StreamEndEvent, { on_spawned_terrain });
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

        // disable_inputs_until_release(world, e, zox_device_mode_none, 1);

        zox_geter_value(game->value, RealmLink, entity, realm);
        zox_geter(realm, SaveGamePath, path);

        float3 spawn_position;
        float3 spawn_euler;
        float4 spawn_rotation;

        byte is_new_game = !has_save_game_file(path->value, "player.dat");

        if (is_new_game) {
            spawn_position = (float3) { 8, 8.5f, 8 };
            spawn_euler = float3_zero;
            spawn_rotation = quaternion_identity;
        } else {
            load_character_p(world, realm, e, &spawn_position, &spawn_euler, &spawn_rotation);
        }

        zox_log("set player character [%fx%fx%f]", spawn_position.x, spawn_position.y, spawn_position.z );

        zox_set(camera->value, Position3D, { spawn_position });
        zox_set(camera->value, Euler, { spawn_euler });
        zox_set(camera->value, Rotation3D, { spawn_rotation });

        // waits for fadeout?
        double delay = game_load_player_delay + game_load_fade_transition_time;

        delay_event(world, &delayed_start_streamer, e, delay);
    }
} zox_sys_end(PlayerGame3StartSystem);


    // delay_event(world, &game_start_player_new_delay, player, 0.02);
    // delay_event(world, &load_player_e, player, 0.02);

    // Spawn our Tunk for terrain generation
    /*int2 tposition = (int2) { cposition.x, cposition.z };
    entity tunk = int2_hashmap_get(chunks2->value, tposition);
    if (!zox_valid(tunk)) {
        tunk = spawn_tunk(world, prefab_tunk2, terrain, tposition, 0);
        if (zox_valid(tunk)) {
            int2_hashmap_add(chunks2->value, tposition, tunk);
            // zox_mut_end(terrain, Chunk2Links);
        }
    }*/

    /*if (!zox_valid(chunk)) {
        chunk = spawn_chunk_terrain(world, prefab_chunk_terrain, terrain, cposition, cposition, terrain_depth, terrain_scale);
        if (zox_valid(chunk)) {
            int3_hashmap_add(chunks->value, cposition, chunk);
        }
    }*/

    // * 2
    /*for (cposition.y = -render_distance_y; cposition.y <= render_distance_y; cposition.y++) {
        entity chunk = int3_hashmap_get(chunks->value, cposition);
        if (!zox_valid(chunk)) {

            chunk = spawn_chunk_terrain(world, prefab_chunk_terrain, terrain, cposition, cposition, terrain_depth, terrain_scale);

            if (zox_valid(chunk)) {
                int3_hashmap_add(chunks->value, cposition, chunk);

                // zox_log("+ spawning chunk for player character loading [%ix%ix%i]:%lu", cposition.x, cposition.y, cposition.z, placer.chunk)
            }
        }
    }*/
    /*if (did_add) {
        zox_mut_end(terrain, ChunkLinks)
    }*/

/*void game_start_player_new_delay(ecs *world, entity player) {

    entity game = zox_get_value(player, GameLink);
    entity realm = zox_get_value(game, RealmLink);
    entity terrain = zox_get_value(realm, TerrainLink);
    entity character = zox_get_value(player, CharacterLink);
    entity camera = zox_get_value(player, CameraLink);

    TerrainPlace placer = find_position_in_terrain(world, terrain);
    if (!zox_valid(placer.chunk)) {
        zox_log_error("+ placement failure: player character placed into [%ix%ix%i]", placer.chunk_position.x, placer.chunk_position.y, placer.chunk_position.z);
    } else {
        zox_set(character, Position3D, { placer.position });
        zox_set(camera, Position3D, { placer.position });
    }

    spawn_player_game_ui(world, player);
    zox_set(character, DisableGravity, { 0 });
    zox_set(character, DisableMovement, { 0 });
}*/