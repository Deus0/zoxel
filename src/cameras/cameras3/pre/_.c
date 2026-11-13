#include "camera3.c"
entity prefab_camera3;
entity prefab_camera_game;
entity prefab_camera_viewport;

void spawn_prefabs_cameras3(ecs* world) {
    prefab_camera3 = spawn_prefab_camera3D(world, prefab_camera);
    prefab_camera_game = spawn_prefab_camera3D(world, prefab_camera);
    prefab_camera_viewport = spawn_prefab_camera_viewport(world, prefab_camera);
}
