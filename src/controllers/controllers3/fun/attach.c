void attach_camera_to_character(ecs *world, entity camera, entity character) {
    if (!zox_valid(character)) {
        zox_log_error("Invalid Character [attach_camera_to_character]");
        return;
    }

    // linking
    zox_set(camera, CharacterLink, { character });
    zox_set(character, CameraLink, { camera });

    // camera
    set_camera_locked(world, camera, character);

    // player
    if (local_mouse) {
        zox_set(local_mouse, MouseLock, { 1 }) // lock mouse since attached
    }
}
