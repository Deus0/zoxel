// NOTE: Sets camera position when body updates
// NOTE: We will use a camera negative Z from now on
zox_sys2(HeadCameraSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkeletonDirty);
    zox_sys_in(HeadBoneLink);
    zox_sys_in(BlockScale);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SkeletonDirty, state);
        zox_sys_i(HeadBoneLink, head);
        zox_sys_i(BlockScale, bscale);
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity camera = zox_get_link(world, e, Camera);
        if (!zox_valid(head->value) ||
            !zox_valid(camera))
        {
            zox_log_error("Invalid Head / Camera on character");
            continue;
        }
        byte camera_state = zox_getv(camera, CameraState);
        if (camera_state != zox_camera_state_first_person) {
            continue;
        }
        zox_geter_value(head->value, BoneSize, float3, head_size);
        float3 camera_offset = (float3) {
            0,
            0,
            - (head_size.z + bscale->value * 1.5f)
        };
        float3 euler = (float3) { 0, 0, 0 };    // 180
        if (is_camera_positive_z) {
            euler.y = 180;
            camera_offset.z *= -1;
        }
        // TODO: Make parenting a system in hierarchy to handle this
        zox_setv(camera, LocalPosition3D, camera_offset);
        // TODO: Remove euler use
        zox_setv(camera, Euler, euler);
        zox_setv(camera, LocalRotation3D,
            quaternion_from_euler(
                float3_scale(
                    euler,
                    degrees_to_radians)));
        zox_set_parent(world, camera, head->value);
        if (dbg_log) {
            zox_log("Set Camera on Head (%s) at [%f]",
                zox_getn(head->value),
                camera_offset.z);
        }
    }
} zox_sys_end(HeadCameraSystem);

