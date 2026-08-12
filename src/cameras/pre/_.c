#include "camera.c"
#include "ui.c"
#include "free.c"
entity prefab_camera;
entity prefab_camera_ui;

void spawn_prefabs_cameras(ecs *world) {
    viewport_scale = 1 / (float) viewport_downscale;
    zox_log("Viewport Scale set [%f] from downscale [%i]", viewport_scale, viewport_downscale);
    prefab_camera = spawn_prefab_camera(world);
    prefab_camera_ui = spawn_prefab_camera_ui(world, prefab_camera);
    spawn_prefab_free_camera(world, prefab_camera);
}
