const float ui_camera_fov = 53.22f;  // fudged this to get ui right... works for now

entity spawn_prefab_camera_ui(ecs *world, entity p) {

    zox_prefab_child(p);
    zox_prefab_name("ui_camera");

    zox_add_tag(e, CameraUI);
    zox_add_tag(e, Camera2D);
    zox_set(e, FieldOfView, { ui_camera_fov });
    zox_set(e, CameraNearDistance, { 0.01f });
    zox_set(e, Position3D, { float3_forward });
    zox_set(e, Rotation3D, { quaternion_identity });

    return e;
}
