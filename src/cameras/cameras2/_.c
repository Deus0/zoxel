zox_tag(CameraFollower2);
#include "pre/camera2.c"
#include "sys/_.c"

void spawn_prefabs_cameras2(ecs* world) {
    spawn_prefab_camera2D_follower(world, prefab_camera);
}

void import_cameras2(ecs* world) {
    zox_module(cameras2);
    zoxd_tag(CameraFollower2);
    zoxd_systems_cameras2(world);
    add_hook_spawn_prefabs(spawn_prefabs_cameras2);
}
