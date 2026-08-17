// NOTE: When exiting game reset the camera!
zox_sys2(PlayerGame3EndSystem) {
    byte dbg_log = 0;
    float3 position = main_menu_camera_position;
    float4 rotation = main_menu_camera_rotation;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(CameraLink);
    zox_sys_out(CharacterLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(PlayerState, state);
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(CameraLink, camera);
        zox_sys_o(CharacterLink, character);
        if (!(dirty->value == zox_dirty_active && state->value == zox_player_state_the_end)) {
            continue;
        }
        if (dbg_log) {
            zox_log("Player [GameEnd] Camera [%s]\n - Destroying Character [%s]", zox_getn(camera->value), zox_getn(character->value));
        }
        if (zox_valid(camera->value)) {
            zox_setv(camera->value, Position3D, position);
            zox_setv(camera->value, Rotation3D, rotation);
            // zox_setv(camera->value, Euler, float3_zero);
            // zox_setv(camera->value, LocalPosition3D, position);
            // zox_setv(camera->value, LocalRotation3D, rotation);
            zox_remove_parent(world, camera->value);
            if (zox_has(camera->value, LocalPosition3D)) {
                zox_remove(camera->value, LocalPosition3D);
            }
            if (zox_has(camera->value, LocalRotation3D)) {
                zox_remove(camera->value, LocalRotation3D);
            }
            zox_setv(camera->value, CharacterLink, 0);
        } else {
            zox_loge("Invalid Camera on Player");
        }
        if (zox_valid(character->value)) {
            zox_delete(character->value);
            character->value = 0;
        } else {
            zox_logw("Player had no character end game");
        }
    }
} zox_sys_end(PlayerGame3EndSystem);


            /*if (state->value == zox_game_state_the_end) {
                zox_setv(e2, PlayerState, zox_player_state_the_end);
                entity camera = zox_getv(e2, CameraLink);
                if (zox_valid(camera)) {
                    zox_set_parent(world, camera, 0);
                    zox_setv(camera, LocalPosition3D, float3_zero);
                    zox_setv(camera, LocalRotation3D, float4_zero);
                    zox_setv(camera, Position3D, float3_zero);
                    zox_setv(camera, Rotation3D, float4_zero);
                }*/
