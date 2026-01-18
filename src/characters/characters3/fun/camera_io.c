void load_camera_e(ecs *world, entity realm, entity camera) {

    zox_geter(realm, SaveGamePath, path);

    load2_camera(path->value, "camera.dat", &camera_save);
    zox_set(camera, Euler, { camera_save.camera_euler })
    zox_set(camera, Rotation3D, { quaternion_from_euler(camera_save.camera_euler) })
    zox_set(camera, LocalRotation3D, { camera_save.camera_rotation_local })
    // zox_muter(camera, LocalRotation3D, camera_rotation3D)
    // camera_rotation3D->value = quaternion_from_euler(camera_save.camera_euler);
}