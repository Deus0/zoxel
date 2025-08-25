void player_end_game3(
    ecs *world,
    const entity player
) {
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    zox_geter_value(player, CanvasLink, entity, canvas);
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