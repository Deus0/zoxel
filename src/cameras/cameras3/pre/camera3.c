entity spawn_prefab_camera3D(ecs *world, const entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("camera3D");
    zox_add_tag(e, Camera3D);
    if (zox_use_post_processing) {
        zox_add_tag(e, RenderCamera);
    }
    return e;
}
