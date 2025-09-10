// NOTE: WE NOW NEED TO SPAWN TERRAIN CHUNK HERE IF IT DOESN"T EXIST!
//      - streaming breaks if all chunks die..

TerrainPlace find_position_in_terrain(
    ecs *world,
    const entity terrain
) {
    const float3 bounds = (float3) { 0.5f, 1.0, 0.5f };
    // zox_geter(terrain, BlockScale, blockScale)
    zox_geter(terrain, ChunkLinks, chunk_links)
    entity chunk = 0;
    int3 chunk_position = int3_zero;
    byte3 local_position = byte3_zero;
    const VoxelNode *voxel_node_above = NULL;
    byte node_depth = 0;
    byte found_position = 0;
    for (int i = render_distance_y; i >= -render_distance_y; i--) {
        chunk_position.y = i;
        chunk = int3_hashmap_get(chunk_links->value, chunk_position);
        if (!zox_valid(chunk)) {
            continue;
        }
        zox_geter(chunk, VoxelNode, chunkd)
        node_depth = zox_get_value(chunk, NodeDepth)
        local_position = find_random_position_on_ground(
            chunkd,
            voxel_node_above,
            node_depth);
        if (!byte3_equals(byte3_full, local_position)) {
            found_position = 1;
            break;
        }
        voxel_node_above = chunkd;
    }
    if (!found_position) {
        zox_log_error("failed finding spawn position for player")
    }
    int chunk_length = powers_of_two[node_depth];
    const int3 chunk_dimensions = (int3) { chunk_length, chunk_length, chunk_length };
    const int3 chunk_voxel_position = get_chunk_positionv(chunk_position, chunk_dimensions);
    const float3 spawn_position = local_to_real_position_character(
        local_position,
        chunk_voxel_position,
        bounds,
        1
    ); // blockScale->value);

    // zox_log("Terrain Place Found [%fx%fx%f]", spawn_position.x, spawn_position.y, spawn_position.z)

    spawn_arrow3D(world, spawn_position, (float3) { 0, 4, 0}, 0.5f, 6, 60);

    return (TerrainPlace) {
        .chunk = chunk,
        .chunk_position = chunk_position,
        .position = spawn_position,
        .rotation = quaternion_identity,
    };
}

void game_start_player_new_delay(
    ecs *world,
    const entity player
) {
    const entity game = zox_get_value(player, GameLink)
    const entity realm = zox_get_value(game, RealmLink)
    const entity terrain = zox_get_value(realm, TerrainLink)
    const entity character = zox_get_value(player, CharacterLink)
    const entity camera = zox_get_value(player, CameraLink)
    TerrainPlace spawn_place = find_position_in_terrain(world, terrain);
    if (!zox_valid(spawn_place.chunk)) {
        zox_log_error("+ placement failure: player character placed into [%ix%ix%i]", spawn_place.chunk_position.x, spawn_place.chunk_position.y, spawn_place.chunk_position.z)
    } else {
        zox_set(character, Position3D, { spawn_place.position })
        zox_set(camera, Position3D, { spawn_place.position })
    }
    spawn_player_game_ui(world, player);
    zox_set(character, DisableGravity, { 0 });
    zox_set(character, DisableMovement, { 0 });
}

entity game_start_player_new(
    ecs *world,
    const entity player
) {
    const entity model = string_hashmap_get(files_hashmap_voxes, new_string_data(player_vox_model));
    if (!model) {
        zox_log_error("File [%s] Not Found.", player_vox_model);
    }
    const entity camera = zox_get_value(player, CameraLink)
    if (!camera) {
        return 0;
    }
    const entity game = zox_get_value(player, GameLink)
    if (!game) {
        return 0;
    }
    const entity realm = zox_get_value(game, RealmLink)
    if (!realm) {
        return 0;
    }
    const entity terrain = zox_get_value(realm, TerrainLink)
    if (!terrain) {
        return 0;
    }
    zox_geter_value(terrain, BlockScale, float, terrain_scale);
    // spawn a column of chunks for new player:
    zox_mut_begin(terrain, ChunkLinks, chunkLinks)
    byte did_add = 0;
    for (int i = -render_distance_y; i <= render_distance_y * 2; i++) {
        int3 chunk_position = (int3) { 0, i, 0 };
        entity chunk = int3_hashmap_get(chunkLinks->value, chunk_position);
        if (!zox_valid(chunk)) {
            chunk = spawn_chunk_terrain(world,
                    prefab_chunk_terrain,
                    terrain,
                    chunk_position,
                    chunk_position,
                    terrain_depth,
                    terrain_scale);
            if (zox_valid(chunk)) {
                int3_hashmap_add(
                    chunkLinks->value,
                    chunk_position,
                    chunk);
                // zox_log("+ spawning chunk for player character loading [%ix%ix%i]:%lu", chunk_position.x, chunk_position.y, chunk_position.z, spawn_place.chunk)
            }
        }
    }
    if (did_add) {
        zox_mut_end(terrain, ChunkLinks)
    }
    // const byte depth = terrain_depth;
    // const int3 chunk_dimensions = int3_single(powers_of_two[depth]);
    const float3 fake_spawn_position = (float3) { 4, 4, 4 };
    // const int3 chunk_position = real_position_to_chunk_position(fake_spawn_position, chunk_dimensions, depth);
    spawn_character3D_data spawn_data = {
        .player = player,
        .model = model,
        .terrain = terrain,
        .rotation = quaternion_identity,
        .position = fake_spawn_position,
        // .chunk_position = chunk_position,
        // .terrain_chunk = spawn_place.chunk,
    };
    const entity e = spawn_character3_player(world, spawn_data);
    delay_event(world, &game_start_player_new_delay, player, 0.5);
    return e;
}

entity game_start_player_load(
    ecs *world,
    const entity player
) {
    const entity model = string_hashmap_get(files_hashmap_voxes, new_string_data(player_vox_model));
    if (!model) {
        zox_log_error("File [%s] Not Found.", player_vox_model);
    }
    const entity camera = zox_get_value(player, CameraLink)
    if (!camera) {
        return 0;
    }
    zox_geter_value(player, GameLink, entity, game);
    if (!game) {
        return 0;
    }
    zox_geter_value(game, RealmLink, entity, realm);
    if (!realm) {
        return 0;
    }
    const entity terrain = zox_get_value(realm, TerrainLink)
    if (!terrain) {
        return 0;
    }
    zox_geter_value(terrain, BlockScale, float, terrain_scale);
    TerrainPlace spawn_place;
    spawn_place.chunk = 0;
    load_character_p(
        world,
        realm,
        player,
        &spawn_place.position,
        &spawn_place.euler,
        &spawn_place.rotation);

    // if character not in chunk, spawn one here
    const byte depth = terrain_depth;
    // const int3 chunk_dimensions = int3_single(powers_of_two[depth]);
    const int3 chunk_position = real_position_to_chunk_position(
        spawn_place.position,
        powers_of_two[depth],
        terrain_scale);

    zox_mut_begin(terrain, ChunkLinks, chunkLinks)
    spawn_place.chunk = int3_hashmap_get(chunkLinks->value, chunk_position);
    // check if exists first
    byte spawned_first_chunk = 0;
    if (!zox_valid(spawn_place.chunk)) {
        spawn_place.chunk = spawn_chunk_terrain(
            world,
            prefab_chunk_terrain,
            terrain,
            chunk_position,
            chunk_position,
            terrain_depth,
            terrain_scale
        );
        spawned_first_chunk = 1;
        if (zox_valid(spawn_place.chunk)) {
            int3_hashmap_add(chunkLinks->value,
                chunk_position,
                spawn_place.chunk);
            zox_mut_end(terrain, ChunkLinks)
            // zox_log("+ spawning chunk for player character loading [%ix%ix%i]:%lu", chunk_position.x, chunk_position.y, chunk_position.z, spawn_place.chunk)
        } else {
            zox_log_error("failed to spawn chunk [%ix%ix%i]:%lu", chunk_position.x, chunk_position.y, chunk_position.z, spawn_place.chunk)
        }
    } else {
        zox_log("+ player character placed into [%ix%ix%i]", chunk_position.x, chunk_position.y, chunk_position.z)
    }
    spawn_character3D_data spawn_data = {
        .model = model,
        .terrain = terrain,
        .terrain_chunk = spawn_place.chunk,
        .chunk_position = chunk_position,
        .position = spawn_place.position,
        .rotation = spawn_place.rotation,
        .euler = spawn_place.euler,
        .player = player,
    };
    zox_set(camera, Position3D, { spawn_place.position }) // reposition camera too
    const entity e = spawn_character3_player(world, spawn_data);
    // assuming we just spawned it
    if (spawned_first_chunk) {
        ChunkEntities entities = (ChunkEntities) { 0 };
        add_to_ChunkEntities(&entities, e);
        zox_set_ptr(spawn_place.chunk, ChunkEntities, entities);
    } else {
        if (zox_valid(spawn_place.chunk)) {
            zox_mut_begin(spawn_place.chunk, ChunkEntities, entityLinks)
            if (add_to_ChunkEntities(entityLinks, e)) {
                zox_mut_end(spawn_place.chunk, ChunkEntities)
            }
        }
    }

    // TODO: LoadCharacterSystem - States
    delay_event(world, &load_player_e, player, 0.1);
    // TODO: Remove use of time, and make a LoadedCharacter Event Hook for this!
    delay_event(world, &spawn_player_game_ui, player, 0.2);

    return e;
}

void player_end_game3(
    ecs *world,
    const entity player
) {
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    // zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CameraLink, entity, camera);
    float3 camera_position = float3_zero;
    float4 camera_rotation = quaternion_identity;
    set_camera_transform_to_main_menu(&camera_position, &camera_rotation, terrain_depth);
    zox_set(camera, Position3D, { camera_position });
    zox_set(camera, Rotation3D, { camera_rotation });
    zox_set(camera, Euler, { float3_zero });
    zox_set(player, CharacterLink, { 0 });
    zox_set(camera, CharacterLink, { 0 });
    // music - attach to game from music module
    play_playlist(world, realm, 0);
    // spawn main menu here
    spawn_main_menu(world, player, game_name);
    zox_geter_value(player, CharacterLink, entity, character);
    if (zox_valid(character)) {
        zox_log_error("Character still alive at end of game.");
        zox_delete(character);
    }
}

// event called when terrain spawned
void on_spawned_terrain(ecs *world, const entity player) {
    const entity game = zox_get_value(player, GameLink);
    const entity realm = zox_get_value(game, RealmLink);
    play_playlist(world, realm, 1);
    // actually we need to do this on loaded player model for bounds
}

// this connects to terrain end stream event and triggers streaming
void link_camera_to_terrain(
    ecs *world,
    const entity player
) {
    const byte depth = terrain_depth;
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
    const entity terrain = zox_get_value(realm, TerrainLink)
    if (!terrain) {
        return;
    }
    zox_geter_value(terrain, BlockScale, float, terrain_scale);
    int3 terrain_position = real_position_to_chunk_position(
        position,
        powers_of_two[depth],
        terrain_scale);

    // if character
    if (game_rule_attach_to_character) {
        zox_geter(realm, SaveGamePath, path);
        byte is_new_game = !has_save_game_file(path->value, "player.dat");
        if (!is_new_game) {
            game_start_player_load(world, player);
        } else {
            game_start_player_new(world, player);
        }
        zox_set(player, PlayerState, { zox_player_state_playing });
    } else {
        set_camera_free(world, camera, 1);
    }
    zox_set(camera, StreamPoint, { terrain_position });
    zox_set(camera, TerrainLink, { terrain });
    zox_set(camera, StreamDirty, { zox_dirty_trigger });
    zox_set(terrain, EventInput, { player });
    zox_set(terrain, StreamEndEvent, { on_spawned_terrain });
    if (is_log_streaming) {
        zox_log("+ terrain spawning started at [%f]", zox_current_time);
    }
}

// spawn character and set camera to streaming terrain
void player_start_game3D(ecs *world, const entity player) {
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    zox_geter_value(player, CameraLink, entity, camera);
    float3 spawn_position = (float3) { 8, 8.5f, 8 };
    float3 spawn_euler = float3_zero;
    float4 spawn_rotation = quaternion_identity;
    zox_geter(realm, SaveGamePath, path);
    byte is_new_game = !has_save_game_file(path->value, "player.dat");
    if (!is_new_game) {
        load_character_p(
            world,
            realm,
            player,
            &spawn_position,
            &spawn_euler,
            &spawn_rotation
        );
    }
    zox_set(camera, Position3D, { spawn_position });
    zox_set(camera, Euler, { spawn_euler });
    zox_set(camera, Rotation3D, { spawn_rotation });
    delay_event(world, &link_camera_to_terrain, player, 0.01f);
}

void player_start_game(
    ecs *world,
    const entity player,
    const byte is_delays
) {
    zox_log("Started!");
    disable_inputs_until_release(world, player, zox_device_mode_none, 1);
    if (zox_game_type == zox_game_mode_3D) {
        delay_event(world, &player_start_game3D, player, game_load_player_delay);
    } else if (zox_game_type == zox_game_mode_2D) {
        delay_event(world, &player_start_game2D_delayed, player, 1.4f);
    }
}

void player_end_game(
    ecs *world,
    const entity player,
    const byte is_delays
) {
    disable_inputs_until_release(world, player, zox_device_mode_none, 1);
    zox_geter_value(player, CanvasLink, entity, canvas);
    find_child_with_tag(canvas, MenuPaused, menu_paused)
    find_child_with_tag(canvas, Taskbar, taskbar)
    if (is_delays) {
        trigger_canvas_fade_transition(world, canvas, end_game_delay_fade, 0.8);
    }
    //if (zox_gett_value(player, DeviceMode) == zox_device_mode_keyboardmouse) {
        if (zox_valid(local_mouse)) {
            zox_set(local_mouse, MouseLock, { 0 })
        }
    //}
    // remove player uis
    if (zox_valid(menu_paused)) {
        zox_delete(menu_paused) // for second player
    }
    if (zox_valid(taskbar)) {
        zox_delete(taskbar)
    }
    if (zox_game_type == zox_game_mode_3D) {
        if (is_delays) {
            delay_event(world, &player_end_game3, player, end_game_delay + 0.4f);
        } else {
            player_end_game3(world, player);
        }
    } else if (zox_game_type == zox_game_mode_2D) {
        delay_event(world, &player_end_game2D, player, end_game_delay);
    }
}

// game state implementation for players module
void players_game_state(
    ecs *world,
    const entity game,
    const byte last_state,
    const byte state
) {
    zox_geter(game, PlayerLinks, players);
    for (int i = 0; i < players->length; i++) {
        const entity player = players->value[i];
        if (state == zox_game_playing_start) {
            zox_set(player, PlayerState, { zox_player_state_loading });
            player_start_game(
                world,
                player,
                is_start_game_delays
            );
        } else if (state == zox_game_start) {
            zox_set(player, PlayerState, { zox_player_state_main_menu });
            player_end_game(
                world,
                player,
                is_end_game_delays
            );
        } else if (state == zox_game_paused) {
            zox_set(player, PlayerState, { zox_player_state_paused });
            pause_player(world, player);
        } else if (last_state == zox_game_paused && state == zox_game_playing) {
            zox_set(player, PlayerState, { zox_game_playing });
            resume_player(world, player);
        }
    }
}