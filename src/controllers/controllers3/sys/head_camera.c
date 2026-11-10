// NOTE: Sets camera position when body updates
// NOTE: We will use a camera negative Z from now on
// NOTE: Runs on skeletons, its owner character holds the CameraLink and BlockScale
void place_camera_head_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity character = zox_get_parent(world, e);
        if (!zox_valid(character) || !zox_has(character, BlockScale)) {
            continue;
        }
        float block_scale = zox_getv(character, BlockScale);
        entity head_bone = zox_get_link(world, e, HeadBoneLink);
        if (!zox_valid(head_bone)) {
            zox_loge("Invalid Head on Skeleton");
            continue;
        }
        entity camera = zox_get_link(world, character, CameraLink);
        if (!zox_valid(camera)) {
            //zox_loge("Invalid Camera on Character %s",
            //    zox_getn(e));
            continue;
        }
        byte camera_state = zox_getv(camera, CameraState);
        if (camera_state != zox_camera_state_first_person) {
            continue;
        }
        float3 head_size = zox_getv(head_bone, BoneSize);
        float3 camera_offset = (float3) {
            0,
            0,
            - (head_size.z + block_scale * 1.5f)
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
        zox_set_parent(world, camera, head_bone);
        if (dbg_log) {
            zox_log("Set Camera on Head (%s) at [%f]",
                zox_getn(head_bone),
                camera_offset.z);
        }
    }
    zox_sys_on_end();
} zoxd_system(place_camera_head_system);

