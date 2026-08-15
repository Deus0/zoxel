entity spawn_prefab_camera3D(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("camera3");
    zox_add(e, Camera3);
    zox_add(e, Perspective);
    if (zox_use_post_processing) {
        zox_add(e, RenderCamera);
        zox_setv(e, CameraBlur, main_menu_blur);
        zox_setv(e, CameraVignette, main_menu_vignette);
    }
    return e;
}
