#include "eye.c"
#include "camera.c"

entity prefab_camera_xr;

void zox_spawn_prefabs_xr(ecs* world) {
    prefab_camera_xr = spawn_prefab_xr_camera(world);
    // spawn a render camera per eye
    /*entity head = zox_new();
    entity left_eye = spawn_xr_camera(world, head);
    entity right_eye = spawn_xr_camera(world, head);*/
}