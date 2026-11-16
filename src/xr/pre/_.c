#include "camera.c"

entity prefab_camera_xr;

void zox_spawn_prefabs_xr(ecs* world) {
    prefab_camera_xr = spawn_prefab_xr_camera(world);
}
