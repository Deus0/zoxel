// todo: put this as a hook in game module
extern entity spawn_character2_player(ecs *world, const entity prefab);

void player_end_game2D(
    ecs *world,
    const entity player
) {
    const entity camera = zox_get_value(player, CameraLink);
    const entity character = zox_get_value(player, CharacterLink);
    // const entity canvas = zox_get_value(player, CanvasLink);
    zox_delete(world_grid2D);
    zox_delete(character);
    zox_set(player, CharacterLink, { 0 });
    zox_set(camera, CharacterLink, { 0 });
    spawn_main_menu(world, player, game_name);
}

void player_start_game2D_delayed(
    ecs *world,
    const entity player
) {
    zox_geter_value(player, CameraLink, entity, camera)
    // set camera2D data
    zox_add_tag(camera, CameraFollower2);
    zox_set(camera, Position3D, { { 0, 0, 1 } });
    zox_set(camera, Rotation3D, { quaternion_from_euler((float3) { 0, 0 * degreesToRadians, 0 }) });
    zox_set(camera, Euler, { { 0, 0 * degreesToRadians, 0 } });
    zox_set(camera, EternalRotation, { float4_identity });
    // spawn character
    const entity character = spawn_character2_player(
        world,
        prefab_game2_player
    );
    // character-camera
    zox_set(character, CameraLink, { camera });
    zox_set(camera, CharacterLink, { character });
    zox_set(camera, CameraTarget, { character });
    zox_set(camera, Character2DLink, { character });
    // character-player
    zox_set(character, PlayerLink, { player });
    zox_set(player, CharacterLink, { character });
}
