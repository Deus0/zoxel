#include "camera2_follow_system.c"

void zoxd_systems_cameras2(ecs *world) {
    zox_system(
        Camera2FollowSystem,
        zoxp_update,
        [out] transforms3.Position3D,
        [out] transforms3.Rotation3D,
        [none] !cameras.Roaming,
        [none] cameras2.CameraFollower2
    );
}