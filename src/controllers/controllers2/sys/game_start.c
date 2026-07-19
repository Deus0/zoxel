extern entity spawn_character2_player(ecs*, entity);

void player_start_game2D_delayed(ecs *world, entity player) {
    zox_log("Player2D Starting now");
    zox_set(player, PlayerState, { zox_player_state_starting });
    zox_set(player, PlayerStateDirty, { zox_dirty_trigger });
    // spawn character
    entity character = spawn_character2_player(world, prefab_game2_player);
    zox_set(character, PlayerLink, { player });
    zox_set(player, CharacterLink, { character });
    zox_geter_value(player, CameraLink, entity, camera);
    if (!zox_valid(camera)) {
        zox_log_error("Camera is gone from player.");
        return;
    }
    // set camera2D data
    zox_add_tag(camera, CameraFollower2);
    zox_set(camera, Position3D, { { 0, 0, 1 } });
    zox_set(camera, Rotation3D, { quaternion_from_euler((float3) { 0, 0 * degreesToRadians, 0 }) });
    zox_set(camera, Euler, { { 0, 0 * degreesToRadians, 0 } });
    zox_set(camera, EternalRotation, { float4_identity });
    // character-camera
    zox_set(character, CameraLink, { camera });
    zox_set(camera, CharacterLink, { character });
    zox_set(camera, CameraTarget, { character });
    zox_set(camera, Character2DLink, { character });
}

zox_sys2(PlayerGame2StartSystem) {
    double game_load_player_delay = 0.2; // 1.1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    // zox_sys_in(GameLink);
    // zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        // zox_sys_i(GameLink, game);
        // zox_sys_i(CameraLink, camera);
        if (!(dirty->value == zox_dirty_active && state->value == zox_player_state_loading)) {
            continue;
        }
        // zox_geter_value(game->value, RealmLink, entity, realm);
        // zox_geter(realm, FolderPath, path);
        // disable_inputs_until_release(world, e, zox_device_mode_none, 1);
        double delay = game_load_player_delay + game_load_fade_transition_time;   // 1.4f
        delay_event(world, &player_start_game2D_delayed, e, delay);
    }
} zox_sys_end(PlayerGame2StartSystem);
