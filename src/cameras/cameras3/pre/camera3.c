entity spawn_prefab_camera3D(ecs *world, entity p) {

    zox_prefab_child(p);
    zox_prefab_name("camera3");

    zox_add_tag(e, Camera3D);

    if (zox_use_post_processing) {
        zox_add_tag(e, RenderCamera);
        zox_prefab_set(e, CameraBlur, { 0 });
    }

    return e;
}
