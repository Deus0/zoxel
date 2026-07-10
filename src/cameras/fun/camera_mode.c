void set_main_cameras(int new_count) {
    main_cameras_count = new_count;
}

CameraSpawnData get_camera_preset(byte mode) {
    CameraSpawnData data = { 0 };
    if (mode == zox_camera_state_topdown) {
        data = camera_preset_top_down;
    } else if (mode == zox_camera_state_ortho) {
        data = camera_preset_ortho;
    } else if (mode == zox_camera_state_first_person) {
        data = camera_preset_first_person;
    } else if (mode == zox_camera_state_third_person) {
        data = camera_preset_third_person;
    } else if (mode == zox_camera_state_2D) {
        data = camera_preset_2D;
    }
    return data;
}

// our main camera transformer
void set_camera_transform(ecs *world, entity camera, entity character, byte mode) {

    if (!zox_valid(camera)) {
        zox_log_error("[set_camera_transform]: Invalid Camera");
        return;
    }

    float3 target_position = (float3) { 8, 0, 8 };
    if (zox_valid(character) && zox_has(character, Position3D)) {
        // zox_log_error("[set_camera_transform]: Invalid Character");
        // return;
        zox_geter_value(character, Position3D, float3, position);
        target_position = position;
    }

    CameraSpawnData data = get_camera_preset(mode);

    if (camera_follow_mode == zox_camera_follow_mode_attach) {
        zox_set(camera, LocalPosition3D, { data.position });
        zox_set(camera, Euler, { data.euler });
        zox_set(camera, LocalRotation3D, { quaternion_from_euler(float3_scale(data.euler, degreesToRadians)) });
    } else {
        zox_set(camera, Position3D, { float3_add(target_position, data.position) });
        zox_set(camera, Rotation3D, { quaternion_from_euler(float3_scale(data.euler, degreesToRadians)) });
    }
}

byte get_camera_state_fov(byte mode) {
    return get_camera_preset(mode).fov;
}

void set_camera_mode(ecs *world, entity e, byte mode) {
    // remove 2 camera modes for now
    if (mode == zox_camera_state_free) {
        mode = zox_camera_state_first_person;
    }
    byte old_camera_follow_mode = camera_follow_mode;
    byte camera_fov = get_camera_state_fov(mode);
    camera_follow_mode = get_camera_preset(mode).follow_mode;
    zox_set(e, CameraState, { mode });
    zox_set(e, FieldOfView, { camera_fov });
    // camera_follow_mode is more complicated, involves how camera is attached to character
    entity target = 0;
    if (old_camera_follow_mode == zox_camera_follow_mode_attach) {
        target = zox_get_parent(world, e); // zox_get_value(e, ParentLink);
    } else {
        target = zox_get_value(e, CameraFollowLink);
    }
    if (old_camera_follow_mode != camera_follow_mode) {
        // remove old link
        if (old_camera_follow_mode == zox_camera_follow_mode_attach) {
            // zox_set(e, ParentLink, { 0 });
            zox_set_parent(world, e, 0);
        } else {
            zox_set(e, CameraFollowLink, { 0 });
        }
        // reattach
        if (camera_follow_mode == zox_camera_follow_mode_attach) {
            zox_set_parent(world, e, target);

        } else if (camera_follow_mode == zox_camera_follow_mode_follow_xz) {
            zox_set(e, CameraFollowLink, { target });
        }
    }
    // set up local positions and rotations
    // use a helper function so attach does the same thing
    set_camera_transform(world, e, target, mode);
}