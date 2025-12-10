void Camera2FollowSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Roaming);
    zox_sys_in(CameraTarget);
    zox_sys_out(Position3D);
    zox_sys_out(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Roaming, canRoam);
        zox_sys_i(CameraTarget, cameraTarget);
        zox_sys_o(Position3D, position3D);
        zox_sys_o(Rotation3D, rotation3D);

        if (canRoam->value || !zox_valid(cameraTarget->value)) {
            continue;
        }

        if (zox_has(cameraTarget->value, Position2)) {
            zox_geter_value(cameraTarget->value, Position2, float2, target_position)
            position3D->value.x = target_position.x;
            position3D->value.y = target_position.y;
        } else if (zox_has(cameraTarget->value, Position3D)) {
            zox_geter_value(cameraTarget->value, Position3D, float3, target_position)
            position3D->value.x = target_position.x;
            position3D->value.y = target_position.y;
        } else {
            continue;
        }
        rotation3D->value = float4_identity;
    }
} zoxd_system2(Camera2FollowSystem);