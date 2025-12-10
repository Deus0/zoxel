void set_camera_free(ecs *world, const entity camera) {
    zox_set(camera, CameraState, { zox_camera_state_free });
    zox_set(camera, CharacterLink, { 0 });

    zox_geter_value(camera, Rotation3D, float4, camera_rotation3D);
    const float3 euler = quaternion_to_euler(camera_rotation3D);
    zox_remove(camera, FirstPersonCamera);
    zox_add_tag(camera, EulerOverride);
    zox_set(camera, Euler, { euler });

    if (camera_follow_mode == zox_camera_follow_mode_attach) {
        zox_set(camera, ParentLink, { 0 })
    } else if (camera_follow_mode == zox_camera_follow_mode_follow_xz) {
        zox_set(camera, CameraFollowLink, { 0 })
    }
    if (local_mouse) {
        zox_set(local_mouse, MouseLock, { 0 });
    }
}