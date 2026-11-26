// a render camera's effects, as the main menu has them (cameras3/_.c), drawn by its pass
void add_prefab_post_processing(ecs* world, entity e) {
    if (!e) {
        return;
    }
    zox_setv(e, CameraBlur, main_menu_blur);
    zox_setv(e, CameraVignette, main_menu_vignette);
    zox_setv(e, CameraTint, 0);
    zox_setv(e, CameraLids, 0);
}

void spawn_prefabs_post_processing(ecs* world) {
    if (!zox_disable_post_processing) {
        add_prefab_post_processing(world, prefab_camera3);
        add_prefab_post_processing(world, prefab_camera_game);
    }
    zox_events_post_processing(world);  // prefab_app is there by then
}
