const float hit_shake_strength = 0.04f;
const double hit_shake_length = 0.2;

// Character was hit - shake the camera of player characters
void hit_camera_shake_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity camera = zox_get_link(world, e, CameraLink);
        spawn_shake_position(
            world,
            camera,
            hit_shake_strength,
            hit_shake_length);
    }
}

// Cleaner - removes the JustHit event tag after hit observers have run
void hit_cleanup_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, JustHit);
    }
}
