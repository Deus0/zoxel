void player_state_game_begin(
    ecs* world,
    entity player,
    byte state)
{
    byte dbg_log = 0;
    if (state != zox_player_state_loading) {
        return;
    }
    entity camera = zox_get_link(world, player, CameraLink);
#ifdef zox_safety_checks
    if (!zox_valid(camera)) {
        zox_loge("Player has no Camera");
        return;
    }
#endif
    entity game = zox_get_parent(world, player);
#ifdef zox_safety_checks
    if (!zox_valid(game)) {
        zox_loge("Player [%s] has invalid Game.",
            zox_getn(player));
        return;
    }
#endif
    entity realm = zox_get_link(world, game, RealmLink);
#ifdef zox_safety_checks
    if (!zox_valid(realm)) {
        zox_loge("Game [%s] has invalid Realm.",
            zox_getn(game));
        return;
    }
    if (!zox_has(realm, FolderPath)) {
        zox_loge("Realm [%s] has invalid Components.",
            zox_getn(realm));
        return;
    }
#endif
    entity terrain = zox_get_link(world, realm, TerrainLink);
#ifdef zox_safety_checks
    if (!zox_valid(terrain)) {
        return;
    }
#endif
    zox_geter(realm, FolderPath, path);
    byte terrain_depth = zox_getv(terrain, NodeDepth);
    float terrain_scale = zox_getv(terrain, BlockScale);
    byte terrain_chunk_length = octree_size(terrain_depth); // powers_of_two[terrain_depth];
    float3 position;
    float3 spawn_euler;
    float4 spawn_rotation;
    byte is_new_game = !has_save_game_file(path->value, "player.dat");
    if (is_new_game) {
        // NOTE: Loads first regions in terrain
        position = (float3) { 8, 8.5f, 8 };
        spawn_euler = float3_zero;
        spawn_rotation = quaternion_identity;
        zox_setv(player, PlayerState, zox_player_state_new);
        zox_setv(player, PlayerStateDirty, zox_dirty_trigger);
        // Regions / Towns only
        zox_set(camera, StreamerLevel, { 0 });
    } else {
        // If has save game
        load_character_player(
            world,
            realm,
            player,
            &position,
            &spawn_euler,
            &spawn_rotation);
        int chunk_position_y = real_position_to_chunk_position1(
            position.y,
            terrain_chunk_length,
            terrain_scale);
        if (!(chunk_position_y >= -render_distance_y && chunk_position_y <= render_distance_y)) {
            zox_logw("Player was out of Terrain Bounds on load");
            position.y = chunk_position_to_real_position1(
                (render_distance_y - 1),
                terrain_chunk_length,
                terrain_scale);
        }
        // NOTE: Loads Regions only
        zox_setv(camera, StreamerLevel, 1);
    }
    // Flag terrain as loading
    // Attach to our terrain's event
    // zox_set(terrain, EventInput, { e });
    zox_add(terrain, Loading);
    // Set Camera
    int3 terrain_position = real_position_to_chunk_position(
        position,
        terrain_chunk_length,
        terrain_scale);
    int2 terrain_position2 = (int2) {
        terrain_position.x,
        terrain_position.z
    };
    entity e2 = camera;
    zox_set(e2, Position3D, { position });
    zox_set(e2, Euler, { spawn_euler });
    zox_set(e2, Rotation3D, { spawn_rotation });
    zox_set(e2, StreamLink, { terrain });
    zox_set(e2, StreamPosition, { terrain_position });
    zox_set(e2, StreamPosition2, { terrain_position2 });
    zox_set(e2, StreamDirty, { zox_dirty_trigger });
    zox_set(e2, StreamDirty2, { zox_dirty_trigger });
    if (dbg_log) {
        zox_log("zox_player_state_new: [%s] on [%s] - new game [%i]",
            zox_getn(player),
            zox_getn(terrain),
            is_new_game);
    }
}
