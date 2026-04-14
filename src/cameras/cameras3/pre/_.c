#include "camera3.c"

void spawn_prefabs_cameras3(ecs* world) {
    prefab_camera_game = spawn_prefab_camera3D(world, prefab_camera);
}
