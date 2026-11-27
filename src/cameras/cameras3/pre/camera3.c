entity spawn_prefab_camera3D(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("camera3");
    zox_add(e, Camera3);
    zox_add(e, Perspective);
    if (!zox_disable_post_processing) {
        zox_add(e, RenderCamera);   // its effects: rendering/cameras/post_processing/pre
    }
    return e;
}

// a 3D camera for an extra view in its part of the screen (spawn_viewport_camera, tst model_views): a render camera,
//  its screen texture on the game view's canvas in its cell (ui/render_textures/sys/screen.c), so it's drawn through
//  the post pass (tonemap, bloom, outlines; no vignette, blur or lids); without post processing or a screen, it draws
//  straight to its part of the screen
entity spawn_prefab_camera_viewport(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("camera_viewport");
    zox_add(e, Camera3);
    zox_add(e, Perspective);
    zox_add(e, CameraViewport);
    if (!zox_disable_post_processing) {
        zox_add(e, RenderCamera);
    }
    return e;
}
