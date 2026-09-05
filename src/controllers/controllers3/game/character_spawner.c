byte find_position_in_terrain(
    ecs* world,
    entity terrain,
    int3 block_position,
    TerrainPlace* output)
{
    float block_scale = zox_getv(terrain, BlockScale);
    byte terrain_depth = zox_getv(terrain, NodeDepth);
    int3 chunk_position = block_position_to_chunk_position(block_position, terrain_depth);
    zox_geter(terrain, ChunkLinks, chunks);
    entity chunk;
    const VoxelNode *voxel_node_above = NULL;
    byte node_depth = 0;
    byte found_position = 0;
    byte3 in_chunk_position = byte3_zero;
    for (sbyte y = render_distance_y; y >= -render_distance_y; y--) {
        chunk_position.y = y;
        chunk = int3_hashmap_get(chunks->value, chunk_position);
        if (!zox_valid(chunk)) {
            zox_loge("Chunk Missing at [0x%ix0]", y);
            continue;
        }
        // If generating, we wait until done
        if (zox_has(chunk, GenerateChunk)) {
            return 0;
        }
        zox_geter(chunk, VoxelNode, chunkd);
        node_depth = zox_getv(chunk, NodeDepth);
        if (find_random_position_on_ground(
            chunkd,
            voxel_node_above,
            node_depth,
            200,
            &in_chunk_position))
        {
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
        return 0;
    }
    float3 positionf = byte3_to_float3(in_chunk_position);
    float3_scale_p(&positionf, block_scale);
    if (zox_valid(chunk)) {
        float3 chunk_positionf = zox_getv(chunk, Position3D);
        float3_add_float3_p(&positionf, chunk_positionf); // chunk
    }
    float3_add_float3_p(&positionf, float3_single(block_scale * 0.5f));
    float4 rotation = quaternion_from_euler(
        (float3) {
            0,
            (rand() % 361) * degreesToRadians,
            0 });
    output->chunk = chunk;
    output->chunk_position = chunk_position;
    output->position = positionf;
    output->rotation = rotation;
    return 1;
}

// NOTE: Here we spawn our new player character
entity game_start_player_new(
    ecs *world,
    entity player,
    entity realm,
    entity terrain,
    entity camera,
    float3* spawned_position,
    byte dbg_log)
{
    lint realm_seed = zox_getv(realm, Seed);
    float terrain_scale = zox_getv(terrain, BlockScale);
    float3 camera_position = zox_getv(camera, Position3D);
    int3 camera_block_position = real_position_to_block_position(camera_position, terrain_scale);
    lint character_seed = seed_rand(realm_seed);
    TerrainPlace placer;
    if (!find_position_in_terrain(world, terrain, camera_block_position, &placer)) {
        return 0;
    }
    *spawned_position = placer.position;
    entity e = spawn_character3_player(
        world,
        prefab_character3_player,
        player,
        realm,
        terrain,
        character_seed,
        placer.position,
        quaternion_identity,
        NULL);
    return e;
}

entity game_start_player_load(
    ecs *world,
    entity player,
    entity realm,
    entity terrain,
    float3* spawned_position,
    byte dbg_log)
{
    // TODO: Load Character Seed
    lint realm_seed = zox_getv(realm, Seed);
    lint character_seed = seed_rand(realm_seed); // , 0, 100000);
    zox_geter_value(terrain, BlockScale, float, terrain_scale);
    zox_geter(terrain, ChunkLinks, chunks);
    TerrainPlace placer;
    placer.chunk = 0;
    // load position for spawning
    load_character_player(world, realm, player, &placer.position, &placer.euler, &placer.rotation);
    byte depth = terrain_depth;
    int3 chunk_position = real_position_to_chunk_position(placer.position, powers_of_two[depth], terrain_scale);
    if (chunk_position.y < -render_distance_y) {
        zox_log("Load Position Y out of Bounds [%i] of [%i]", chunk_position.y, render_distance_y);
        chunk_position.y = render_distance_y - 1;
    }
    if (chunk_position.y > render_distance_y) {
        zox_log("Load Position Y out of Bounds [%i] of [%i]", chunk_position.y, render_distance_y);
        chunk_position.y = render_distance_y - 1;
    }
    placer.chunk = int3_hashmap_get(chunks->value, chunk_position);
    // Still loading
    if (!zox_valid(placer.chunk)) {
        if (dbg_log) {
            zox_log("   - Chunk Invalid at [%ix%ix%i]", chunk_position.x, chunk_position.y, chunk_position.z);
        }
        return 0;
    }
    if (zox_has(placer.chunk, GenerateChunk)) {
        if (dbg_log) {
            zox_log("   - Chunk is Generating at [%ix%ix%i]", chunk_position.x, chunk_position.y, chunk_position.z);
        }
        return 0;
    }
    *spawned_position = placer.position;
    entity e = spawn_character3_player(
        world,
        prefab_character3_player,
        player,
        realm,
        terrain, character_seed,
        placer.position,
        quaternion_identity,
        NULL);
    return e;
}

// NOTE: Player Spawns Player Character
// TODO: Remove terrain load checks out this, and just change player from game when terrain is loaded
zox_sys2(PlayerBeginSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(CharacterLink);
    zox_sys_out(PlayerState);
    zox_sys_out(PlayerStateDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(CharacterLink, character);
        zox_sys_o(PlayerState, state);
        zox_sys_o(PlayerStateDirty, dirty);
        // Now Spawning Character
        if (state->value != zox_player_state_starting) {
            continue;
        }
        entity game = zox_get_parent(world, e);
        entity realm = zox_getv(game, RealmLink);
#ifdef zox_safety_checks
        if (!zox_valid(realm)) {
            zox_loge("Player has Invalid Realm");
            continue;
        }
#endif
        entity terrain = zox_get_child_by_id(
            world,
            realm,
            zox_id(Terrain));
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_loge("Player has Invalid Terrain");
            continue;
        }
#endif
        // Wait for Terrain to load
        if (zox_has(terrain, Loading)) {
            if (dbg_log) {
                zox_log("Terrain is still Loading [%s]",
                    zox_getn(terrain));
            }
            // Keep active
            dirty->value = zox_dirty_trigger;
            continue;
        }
        if (zox_valid(character->value)) {
            zox_loge("Trying to load character twice [zox_player_state_starting]");
            state->value = zox_player_state_play_trigger; // zox_player_state_play_begin;
            dirty->value = zox_dirty_trigger;
            continue;
        }
        // actually we need to do this on loaded player model for bounds
        // if character
        zox_geter(realm, FolderPath, path);
        byte is_new_game = !has_save_game_file(path->value, "player.dat");
        float3 spawn_position;
        if (!is_new_game) {
            character->value = game_start_player_load(
                world,
                e,
                realm,
                terrain,
                &spawn_position,
                dbg_log);
            if (!character->value) {
                if (dbg_log) {
                    zox_log("[%s] Terrain Position not ready for load game", zox_getn(e));
                }
                continue;
            }
        } else {
            entity camera = zox_get_link(world, e, Camera);
            character->value = game_start_player_new(
                world,
                e,
                realm,
                terrain,
                camera,
                &spawn_position,
                dbg_log);
            if (!character->value) {
                if (dbg_log) {
                    zox_log("[%s] Terrain Position not found for new game", zox_getn(e));
                }
                continue;
            }
        }
        if (dbg_log) {
            zox_log("[%s] Player Character Spawned at [%fx%fx%f]",
                is_new_game ? "New" : "Load",
                spawn_position.x,
                spawn_position.y,
                spawn_position.z);
        }
        spawn_arrow3D(
            world,
            spawn_position,
            (float3) { 0, 1, 0}, 0.2f, 6, 30);
        // Needs ui spawn after frame
        play_playlist(world, realm, 1);
        entity mouse = zox_get_child_by_id(world, e, zox_id(Mouse));
        if (mouse) {
            zox_setv(mouse, MouseLock, 1);
        }
        state->value = zox_player_state_play_trigger;
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(PlayerBeginSystem);
