void Camera3FollowSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LocalPosition3D);
    zox_sys_out(Position3D);
    for (int i = 0; i < it->count; i++) {
        entity cameraFollowLink = zox_get_link(world, it->entities[i], CameraFollowLink);
        zox_sys_i(LocalPosition3D, localPosition3D);
        zox_sys_o(Position3D, position);
        if (!cameraFollowLink) {
            continue;
        }
        if (zox_has(cameraFollowLink, Position2)) {
            float2 target_position = zox_getv(cameraFollowLink, Position2);
            position->value.x = target_position.x;
            position->value.y = target_position.y;
            position->value.z = 0;
        } else if (zox_has(cameraFollowLink, Position3D)) {
            position->value = zox_getv(cameraFollowLink, Position3D);
        } else {
            continue;
        }
        float3_add_float3_p(&position->value, localPosition3D->value);
    }
    zox_sys_on_end();
} zoxd_system(Camera3FollowSystem);
