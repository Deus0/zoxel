const float ui_camera_fov = 53.22f;  // fudged this to get ui right... works for now

entity spawn_prefab_camera_ui(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("ui_camera");
    zox_add(e, CameraUI);
    zox_add(e, Camera2);
    zox_add(e, Orthographic);
    // zox_set(e, FieldOfView, { ui_camera_fov });
    zox_setv(e, CameraNearDistance, 0);
    // zox_setv(e, Position3D, float3_forward);
    // zox_setv(e, Rotation3D, quaternion_identity);
    return e;
}
