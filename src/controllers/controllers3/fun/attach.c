void set_camera_locked(ecs *world, const entity camera, const entity target) {
    zox_set(camera, CameraState, { zox_camera_state_first_person });
    zox_add_tag(camera, FirstPersonCamera);
    zox_set(camera, Roaming, { 0 });
    if (camera_follow_mode == zox_camera_follow_mode_attach) {
        zox_set(camera, ParentLink, { target });
    } else if (camera_follow_mode == zox_camera_follow_mode_follow_xz) {
        zox_set(camera, CameraFollowLink, { target });
    }
    // zox_set(camera, EternalRotation, { quaternion_identity })
    zox_remove(camera, EulerOverride);
    set_camera_transform(
        world,
        camera,
        target,
        zox_camera_state_first_person);
}

void attach_camera_to_character(ecs *world, const entity camera, const entity character) {
    if (!zox_valid(character)) {
        zox_log_error("character invalid in attach_camera_to_character")
        return;
    }
    // character
    // zox_set(character, DisableMovement, { 0 })
    // linking
    zox_set(camera, CharacterLink, { character })
    // camera
    set_camera_locked(world, camera, character);
    // player
    if (local_mouse) {
        zox_set(local_mouse, MouseLock, { 1 }) // lock mouse since attached
    }
}