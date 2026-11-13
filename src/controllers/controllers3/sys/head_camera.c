// NOTE: Sets camera position when body updates
// NOTE: We will use a camera negative Z from now on
// NOTE: Runs on skeletons, its owner character holds the CameraLink and BlockScale

// the camera at the eyes, in front of the face (+z), and in front of whatever is under it (a thick chest, a shirt):
//  bones in the head's column (covering its middle on x) push it forward, so looking down doesn't see inside the
//  body. Bones' sizes are half their size, from BonePosition (the skeleton's pose)
float3 head_camera_offset(ecs* world, entity skeleton, entity head_bone, float block_scale) {
    const float3 head_position = zox_getv(head_bone, BonePosition);
    const float3 head_size = zox_getv(head_bone, BoneSize);
    float front = head_position.z + head_size.z;
    const BoneLinks* bones = zox_get(skeleton, BoneLinks);
    for (int i = 0; bones && i < bones->length; i++) {
        const entity bone = bones->value[i];
        if (!zox_valid(bone) || !zox_has(bone, BoneSize)) {
            continue;
        }
        const float3 position = zox_getv(bone, BonePosition);
        const float3 size = zox_getv(bone, BoneSize);
        if (fabsf(position.x - head_position.x) < size.x && position.z + size.z > front) {
            front = position.z + size.z;
        }
    }
    // eyes a little above the head's middle (parts/ins/head.c), a voxel in front of the face
    const float z = front - head_position.z + block_scale;
    return (float3) { 0, head_size.y * 0.15f, is_camera_positive_z ? z : -z };
}

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
        const float3 camera_offset = head_camera_offset(world, e, head_bone, block_scale);
        float3 euler = (float3) { 0, 0, 0 };    // 180
        if (is_camera_positive_z) {
            euler.y = 180;
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

