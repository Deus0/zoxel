// NOTE: Sets camera position when body updates
// NOTE: We will use a camera negative Z from now on
zox_sys2(HeadCameraSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkeletonDirty);
    zox_sys_in(HeadBoneLink);
    zox_sys_in(CameraLink);
    zox_sys_in(BlockScale);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(SkeletonDirty, state);
        zox_sys_i(HeadBoneLink, head);
        zox_sys_i(CameraLink, camera);
        zox_sys_i(BlockScale, bscale);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(head->value) || !zox_valid(camera->value)) {
            zox_log_error("Invalid Head / Camera on character");
            continue;
        }
        byte camera_state = zox_getv(camera->value, CameraState);
        if (camera_state != zox_camera_state_first_person) {
            continue;
        }
        zox_geter_value(head->value, BoneSize, float3, head_size);
        float3 camera_offset = (float3) { 0, 0, - (head_size.z + bscale->value * 1.5f) };
        float3 euler = (float3) { 0, 0, 0 };    // 180
        if (is_camera_positive_z) {
            euler.y = 180;
            camera_offset.z *= -1;
        }
        // TODO: Make parenting a system in hierarchy to handle this
        zox_set_parent(world, camera->value, head->value);
        zox_set(camera->value, LocalPosition3D, { camera_offset });
        // TODO: Remove euler use
        zox_set(camera->value, Euler, { euler });
        zox_set(camera->value, LocalRotation3D, { quaternion_from_euler(float3_scale(euler, degrees_to_radians)) });
        if (dbg_log) {
            zox_log("Set Camera on Head (%s) at [%f]", zox_get_name(head->value), camera_offset.z);
        }
    }
} zox_sys_end(HeadCameraSystem);

