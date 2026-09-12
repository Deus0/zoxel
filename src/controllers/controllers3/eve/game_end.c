void player_state_game_end(
    ecs* world,
    entity player,
    byte state)
{
    byte dbg_log = 0;
    float3 position = main_menu_camera_position;
    float4 rotation = main_menu_camera_rotation;
    if (state != zox_player_state_the_end) {
        return;
    }
    entity camera = zox_get_link(world, player, CameraLink);
    entity character = zox_get_link(world, player, Character);
    if (dbg_log) {
        zox_log("Player [GameEnd] Camera [%s]\n - Destroying Character [%s]",
            zox_getn(camera),
            zox_getn(character));
    }
    if (zox_valid(camera)) {
        zox_setv(camera, Position3D, position);
        zox_setv(camera, Rotation3D, rotation);
        zox_remove_parent(world, camera);
        if (zox_has(camera, LocalPosition3D)) {
            zox_remove(camera, LocalPosition3D);
        }
        if (zox_has(camera, LocalRotation3D)) {
            zox_remove(camera, LocalRotation3D);
        }
        // zox_unlink(world, camera, Character, character);
        // zox_setv(camera, CharacterLink, 0);
    } else {
        zox_loge("[zox_player_state_the_end] Invalid [camera]");
    }
    if (zox_valid(character)) {
        zox_delete(character);
        // zox_setv(player, CharacterLink, 0);
    } else {
        zox_loge("[zox_player_state_the_end] Invalid [character]");
    }
}

