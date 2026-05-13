void key_down_toggle_streaming(ecs* world, int32_t keycode) {
    if (keycode != zox_key_F3) {
        return;
    }
    zox_cameras_disable_streaming = !zox_cameras_disable_streaming;
    zox_log("+ streaming is [%s]", zox_cameras_disable_streaming ? "disabled" : "enabled");
    // spawn_sound_from_file_index(world, prefab_sound, 0);
}
