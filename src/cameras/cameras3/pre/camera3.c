entity spawn_prefab_camera3D(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("camera3");
    zox_add(e, Camera3);
    zox_add(e, Perspective);
    if (!zox_disable_post_processing) {
        zox_add(e, RenderCamera);
        zox_setv(e, CameraBlur, main_menu_blur);
        zox_setv(e, CameraVignette, main_menu_vignette);
    }
    return e;
}

// a 3D camera that draws straight to its part of the screen (no render texture, no post processing)
//  - for extra views: spawn_viewport_camera, tst model_views
entity spawn_prefab_camera_viewport(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("camera_viewport");
    zox_add(e, Camera3);
    zox_add(e, Perspective);
    return e;
}
