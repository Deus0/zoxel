zox_sys2(HeadCameraSystem) {
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

        zox_geter_value(head->value, BoneSize, float3, head_size);
        float3 camera_offset = (float3) { 0, 0, head_size.z + bscale->value };

        zox_set(camera->value, ParentLink, { head->value });
        zox_set(camera->value, LocalPosition3D, { camera_offset });

        // zox_log("Set Camera on Head (%s) at [%f]", zox_get_name(head->value), camera_offset.z);
    }
} zox_sys_end(HeadCameraSystem);

