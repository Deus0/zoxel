void player_end_game3(
    ecs *world,
    const entity player
) {
    zox_geter_value(player, CameraLink, entity, camera);
}

void PlayerGame3EndSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(CameraLink);
    zox_sys_in(CharacterLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(CameraLink, camera);
        zox_sys_i(CharacterLink, character);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (state->value != zox_player_state_main_menu) {
            continue;
        }

        float3 camera_position = float3_zero;
        float4 camera_rotation = quaternion_identity;
        set_camera_transform_to_main_menu(&camera_position, &camera_rotation, terrain_depth);

        zox_set(camera->value, Position3D, { camera_position });
        zox_set(camera->value, Rotation3D, { camera_rotation });
        zox_set(camera->value, Euler, { float3_zero });
        zox_set(camera->value, CharacterLink, { 0 });

        if (zox_valid(character->value)) {
            zox_logw("Character still alive at end of game.");
            zox_delete(character->value);
            zox_set(e, CharacterLink, { 0 });
        }
    }
} zoxd_system2(PlayerGame3EndSystem);