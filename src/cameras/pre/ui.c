const float ui_camera_fov = 53.22f;  // fudged this to get ui right... works for now

entity spawn_prefab_camera_ui(
    ecs *world,
    entity prefab)
{
    entity e = zox_prefab_from_parent(world, prefab);
    zox_prefab_name("ui_camera");
    zox_add(e, CameraUI);
    zox_add(e, Camera2);
    zox_add(e, Orthographic);
    zox_setv(e, CameraNearDistance, 0);
    return e;
}
